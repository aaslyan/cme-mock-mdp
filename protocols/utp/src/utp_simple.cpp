#include "../include/utp_simple.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace utp_simple {

UTPMessageSender::UTPMessageSender(const std::string& ip, int port)
    : m_multicast_ip(ip)
    , m_port(port)
{
}

UTPMessageSender::~UTPMessageSender()
{
    cleanup();
}

bool UTPMessageSender::initialize()
{
    // Create UDP socket
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::cerr << "Failed to create UTP socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Enable socket reuse
    int reuse = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
        return false;
    }

    // Set TTL for multicast
    int ttl = 1;
    if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        std::cerr << "Failed to set multicast TTL: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "UTP transport initialized for " << m_multicast_ip << ":" << m_port << std::endl;
    return true;
}

void UTPMessageSender::cleanup()
{
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
}

bool UTPMessageSender::send_heartbeat()
{
    AdminHeartbeat heartbeat;
    std::vector<uint8_t> buffer;
    size_t size = heartbeat.pack(buffer);

    return send_multicast_message(buffer.data(), size);
}

bool UTPMessageSender::send_security_definition(uint32_t security_id, const std::string& symbol)
{
    SecurityDefinition secDef;
    secDef.securityUpdateAction = SecurityUpdateAction::ADD;
    secDef.lastUpdateTime = get_current_timestamp_ns();

    // Set symbol
    std::memset(secDef.symbol, 0, sizeof(secDef.symbol));
    std::strncpy(secDef.symbol, symbol.c_str(),
        std::min(symbol.length(), sizeof(secDef.symbol) - 1));

    secDef.securityID = security_id;
    secDef.securityIDSource = 1; // Exchange assigned
    secDef.securityType = MarketDataType::FXSPOT;

    // Set default currencies
    std::memcpy(secDef.currency1, "USD", 3);
    std::memcpy(secDef.currency2, "EUR", 3);

    secDef.ratePrecision = 5;
    secDef.depthOfBook = 10;
    secDef.minTradeVol = 1000;

    std::vector<uint8_t> buffer;
    size_t size = secDef.pack(buffer);

    return send_multicast_message(buffer.data(), size);
}

bool UTPMessageSender::send_snapshot(uint32_t security_id, double bid_price, double ask_price,
    int64_t bid_size, int64_t ask_size)
{
    MDFullRefresh snapshot;
    snapshot.lastMsgSeqNumProcessed = m_sequence++;
    snapshot.securityID = security_id;
    snapshot.rptSeq = m_sequence++;
    snapshot.transactTime = get_current_timestamp_ns();

    std::memset(snapshot.mdEntryOriginator, 0, sizeof(snapshot.mdEntryOriginator));
    std::strncpy(snapshot.mdEntryOriginator, "UTP_UNIFIED", 15);

    snapshot.marketDepth = 5;
    snapshot.securityType = MarketDataType::FXSPOT;

    // Add bid and ask entries
    MDEntry bid_entry, ask_entry;

    bid_entry.mdEntryType = MDEntryType::BID;
    bid_entry.mdEntryPx.mantissa = static_cast<int64_t>(bid_price * 1e9);
    bid_entry.mdEntrySize = bid_size;

    ask_entry.mdEntryType = MDEntryType::OFFER;
    ask_entry.mdEntryPx.mantissa = static_cast<int64_t>(ask_price * 1e9);
    ask_entry.mdEntrySize = ask_size;

    snapshot.mdEntries.push_back(bid_entry);
    snapshot.mdEntries.push_back(ask_entry);

    std::vector<uint8_t> buffer;
    size_t size = snapshot.pack(buffer);

    return send_multicast_message(buffer.data(), size);
}

bool UTPMessageSender::send_incremental_update(uint32_t security_id, double bid_price, double ask_price,
    int64_t bid_size, int64_t ask_size)
{
    MDIncrementalRefresh incremental;
    incremental.securityID = security_id;
    incremental.rptSeq = m_sequence++;
    incremental.transactTime = get_current_timestamp_ns();

    std::memset(incremental.mdEntryOriginator, 0, sizeof(incremental.mdEntryOriginator));
    std::strncpy(incremental.mdEntryOriginator, "UTP_UNIFIED", 15);

    // Add bid update
    MDIncrementalEntry bid_entry;
    bid_entry.mdUpdateAction = MDUpdateAction::CHANGE;
    bid_entry.mdEntryType = MDEntryType::BID;
    bid_entry.mdEntryPx.mantissa = static_cast<int64_t>(bid_price * 1e9);
    bid_entry.mdEntrySize = bid_size;

    // Add ask update
    MDIncrementalEntry ask_entry;
    ask_entry.mdUpdateAction = MDUpdateAction::CHANGE;
    ask_entry.mdEntryType = MDEntryType::OFFER;
    ask_entry.mdEntryPx.mantissa = static_cast<int64_t>(ask_price * 1e9);
    ask_entry.mdEntrySize = ask_size;

    incremental.mdEntries.push_back(bid_entry);
    incremental.mdEntries.push_back(ask_entry);

    std::vector<uint8_t> buffer;
    size_t size = incremental.pack(buffer);

    return send_multicast_message(buffer.data(), size);
}

bool UTPMessageSender::send_multicast_message(const void* data, size_t size)
{
    if (m_socket < 0) {
        std::cerr << "Socket not initialized" << std::endl;
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);

    if (inet_pton(AF_INET, m_multicast_ip.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Invalid multicast address: " << m_multicast_ip << std::endl;
        return false;
    }

    ssize_t sent = sendto(m_socket, data, size, 0,
        (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        std::cerr << "Failed to send UTP message: " << strerror(errno) << std::endl;
        return false;
    }

    return sent == static_cast<ssize_t>(size);
}

uint64_t UTPMessageSender::get_current_timestamp_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count();
}

} // namespace utp_simple