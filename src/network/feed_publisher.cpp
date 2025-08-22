#include "network/feed_publisher.h"
#include "messages/cme_sbe_encoder.h"
#include "messages/message_factory.h"
#include "messages/sbe_encoder.h"
#include "utils/hexdump.h"
#include "utils/logger.h"
#include "utils/packet_verifier.h"
#include <algorithm>
#include <sstream>

// Global verbose flag definition
bool g_verbose_mode = false;

namespace cme_mock {

// Base FeedPublisher implementation
FeedPublisher::FeedPublisher(const std::string& ip, uint16_t port,
    std::shared_ptr<OrderBookManager> book_manager)
    : book_manager_(book_manager)
{
    udp_publisher_ = std::make_shared<UDPPublisher>(ip, port);
}

bool FeedPublisher::initialize()
{
    if (!udp_publisher_->initialize()) {
        LOG_ERROR("Failed to initialize UDP publisher");
        return false;
    }
    return true;
}

// SnapshotFeedPublisher implementation
void SnapshotFeedPublisher::publish_all_snapshots()
{
    auto security_ids = book_manager_->get_all_security_ids();

    LOG_INFO("Publishing snapshots for " + std::to_string(security_ids.size()) + " instruments");

    for (auto security_id : security_ids) {
        publish_snapshot(security_id);
    }
}

void SnapshotFeedPublisher::publish_snapshot(uint32_t security_id)
{
    auto book = book_manager_->get_order_book(security_id);
    if (!book) {
        LOG_WARNING("Order book not found for security " + std::to_string(security_id));
        return;
    }

    // Create snapshot message from order book
    auto snapshot = create_snapshot_from_book(*book, snapshot_sequence_ - 1, snapshot_sequence_);

    // Set packet header info
    snapshot.header.sequence_number = snapshot_sequence_++;
    snapshot.header.sending_time = get_timestamp_ns();
    snapshot.header.msg_count = 1;

    // Encode and send
    auto encoded = encode_snapshot(snapshot);

    // Print hex data for debugging
    std::cout << "SERVER SENDING SNAPSHOT (" << encoded.size() << " bytes): ";
    for (size_t i = 0; i < std::min(encoded.size(), size_t(32)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)encoded[i] << " ";
    }
    std::cout << std::dec << std::endl;

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent snapshot for " << book->get_symbol()
           << " (ID: " << security_id << ")"
           << " - " << snapshot.entries.size() << " entries"
           << ", " << encoded.size() << " bytes";
        LOG_DEBUG(ss.str());
    } else {
        LOG_ERROR("Failed to send snapshot for security " + std::to_string(security_id));
    }
}

std::vector<uint8_t> SnapshotFeedPublisher::encode_snapshot(const SnapshotFullRefresh& snapshot)
{
    // Use the centralized encoder for proper packet structure
    auto message = CMESBEEncoder::encode_snapshot_full_refresh(snapshot);

    // Create a single-message packet
    std::vector<std::vector<uint8_t>> messages = { message };

    return CMESBEEncoder::encode_multi_message_packet(
        snapshot.header.sequence_number,
        snapshot.header.sending_time,
        messages);
}

// IncrementalFeedPublisher implementation
void IncrementalFeedPublisher::publish_book_update(uint32_t security_id, const MDPriceLevel& level)
{
    IncrementalRefresh update;
    update.header.sequence_number = sequence_number_++;
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 1;
    update.transact_time = get_timestamp_ns();

    update.price_levels.push_back(level);

    auto encoded = encode_incremental(update);

    // Verbose packet logging and verification
    if (g_verbose_mode) {
        std::cout << "\n=== SERVER SENDING INCREMENTAL UPDATE ===" << std::endl;
        std::cout << "Packet size: " << encoded.size() << " bytes" << std::endl;
        std::cout << "Security ID: " << security_id << std::endl;
        std::cout << "Price levels: " << update.price_levels.size() << std::endl;
        std::cout << "Trades: " << update.trades.size() << std::endl;

        // Show detailed packet content
        hexdump(encoded, "Complete Packet");

        // Verify packet structure
        PacketVerifier::verify_and_log(encoded, "Incremental Update Packet");
        std::cout << "==========================================\n"
                  << std::endl;
    } else {
        // Compact output for non-verbose mode
        std::cout << "SERVER SENDING INCREMENTAL (" << encoded.size() << " bytes): ";
        for (size_t i = 0; i < std::min(encoded.size(), size_t(16)); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)encoded[i] << " ";
        }
        if (encoded.size() > 16)
            std::cout << "...";
        std::cout << std::dec << std::endl;
    }

    if (udp_publisher_->send(encoded)) {
        LOG_DEBUG("Sent book update for security " + std::to_string(security_id));
    }
}

void IncrementalFeedPublisher::publish_trade(uint32_t security_id, const MDTrade& trade)
{
    IncrementalRefresh update;
    update.header.sequence_number = sequence_number_++;
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 1;
    update.transact_time = get_timestamp_ns();

    update.trades.push_back(trade);

    auto encoded = encode_incremental(update);

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent trade for security " << security_id
           << " - " << trade.quantity << " @ " << (trade.price / 100.0);
        LOG_DEBUG(ss.str());
    }
}

void IncrementalFeedPublisher::publish_batch(const IncrementalRefresh& update)
{
    // Copy and update header
    IncrementalRefresh batch = update;
    batch.header.sequence_number = sequence_number_++;
    batch.header.sending_time = get_timestamp_ns();
    batch.header.msg_count = update.price_levels.size() + update.trades.size();

    auto encoded = encode_incremental(batch);

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent batch update - "
           << update.price_levels.size() << " price levels, "
           << update.trades.size() << " trades";
        LOG_DEBUG(ss.str());
    }
}

std::vector<uint8_t> IncrementalFeedPublisher::encode_incremental(const IncrementalRefresh& update)
{
    // Encode complete MDP packet with proper CME format
    auto packet_header = CMESBEEncoder::encode_packet_header(
        update.header.sequence_number,
        update.header.sending_time);

    std::vector<uint8_t> result;
    result.reserve(1024); // Reserve space for efficiency

    // 1. Add Binary Packet Header (12 bytes)
    result.insert(result.end(), packet_header.begin(), packet_header.end());

    // CME MDP 3.0 expects each update as a separate message
    // Process each price level as a separate message
    for (const auto& level : update.price_levels) {
        IncrementalRefresh single_update;
        single_update.header = update.header;
        single_update.transact_time = update.transact_time;
        single_update.price_levels.push_back(level);

        auto message = CMESBEEncoder::encode_incremental_refresh(single_update);

        // 2. Add Message Size (2 bytes, little-endian) - total message length
        uint16_t message_size = static_cast<uint16_t>(message.size());
        result.push_back(message_size & 0xFF);
        result.push_back((message_size >> 8) & 0xFF);

        // 3. Add SBE Message (contains Block Length + Template ID + Schema ID + Version + Body)
        result.insert(result.end(), message.begin(), message.end());
    }

    // Process each trade as a separate message
    for (const auto& trade : update.trades) {
        IncrementalRefresh single_update;
        single_update.header = update.header;
        single_update.transact_time = update.transact_time;
        single_update.trades.push_back(trade);

        auto message = CMESBEEncoder::encode_incremental_refresh(single_update);

        // 2. Add Message Size (2 bytes, little-endian) - total message length
        uint16_t message_size = static_cast<uint16_t>(message.size());
        result.push_back(message_size & 0xFF);
        result.push_back((message_size >> 8) & 0xFF);

        // 3. Add SBE Message (contains Block Length + Template ID + Schema ID + Version + Body)
        result.insert(result.end(), message.begin(), message.end());
    }

    // If no updates, create at least one empty message
    if (update.price_levels.empty() && update.trades.empty()) {
        auto message = MDPMessageEncoder::encode_incremental_refresh(update);

        // 2. Add Message Size (2 bytes, little-endian) - total message length
        uint16_t message_size = static_cast<uint16_t>(message.size());
        result.push_back(message_size & 0xFF);
        result.push_back((message_size >> 8) & 0xFF);

        // 3. Add SBE Message (contains Block Length + Template ID + Schema ID + Version + Body)
        result.insert(result.end(), message.begin(), message.end());
    }

    return result;
}

} // namespace cme_mock