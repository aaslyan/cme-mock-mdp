#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "utils/hexdump.h"
#include "utils/packet_verifier.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace cme_mock;
using namespace cme_sbe;

class CMETestClient {
private:
    int socket_fd;
    bool verbose;

public:
    CMETestClient(bool verbose_mode = false)
        : socket_fd(-1)
        , verbose(verbose_mode)
    {
    }

    ~CMETestClient()
    {
        if (socket_fd >= 0) {
            close(socket_fd);
        }
    }

    bool connect_to_server(const std::string& ip, uint16_t port)
    {
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            return false;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());

        if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error binding to " << ip << ":" << port << std::endl;
            return false;
        }

        std::cout << "✅ Client listening on " << ip << ":" << port << std::endl;
        return true;
    }

    void process_packets()
    {
        std::cout << "\n=== CME Test Client Started ===\n"
                  << std::endl;
        std::cout << "Processing packets from mock server..." << std::endl;
        std::cout << "Press Ctrl+C to stop\n"
                  << std::endl;

        uint8_t buffer[2048];
        int packet_count = 0;

        while (true) {
            ssize_t bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0);

            if (bytes_received < 0) {
                std::cerr << "Error receiving data" << std::endl;
                break;
            }

            if (bytes_received == 0) {
                continue;
            }

            packet_count++;

            std::cout << "\n=== Received Packet #" << packet_count << " ===" << std::endl;
            std::cout << "Raw packet size: " << bytes_received << " bytes" << std::endl;

            // Convert to vector for processing
            std::vector<uint8_t> packet(buffer, buffer + bytes_received);

            if (verbose) {
                hexdump(packet, "Raw Packet Data");
            }

            // Process packet exactly like a real CME client
            bool parsed = parse_cme_packet(packet);

            if (parsed) {
                std::cout << "✅ Packet parsed successfully" << std::endl;
            } else {
                std::cout << "❌ Packet parsing failed" << std::endl;
            }

            std::cout << "========================================\n"
                      << std::endl;
        }
    }

private:
    bool parse_cme_packet(const std::vector<uint8_t>& packet)
    {
        try {
            size_t offset = 0;

            // Step 1: Parse packet header (12 bytes)
            if (packet.size() < 12) {
                std::cerr << "Packet too small for header" << std::endl;
                return false;
            }

            uint32_t sequence_number = packet[0] | (packet[1] << 8) | (packet[2] << 16) | (packet[3] << 24);

            uint64_t sending_time = 0;
            for (int i = 0; i < 8; ++i) {
                sending_time |= (static_cast<uint64_t>(packet[4 + i]) << (i * 8));
            }

            std::cout << "Packet Header:" << std::endl;
            std::cout << "  Sequence: " << sequence_number << std::endl;
            std::cout << "  Timestamp: " << sending_time << std::endl;

            offset = 12;
            int message_count = 0;

            // Step 2: Parse messages
            while (offset < packet.size()) {
                if (offset + 2 > packet.size()) {
                    std::cerr << "Incomplete message size field" << std::endl;
                    return false;
                }

                // Read message size
                uint16_t message_size = packet[offset] | (packet[offset + 1] << 8);
                offset += 2;

                message_count++;
                std::cout << "\nMessage " << message_count << ":" << std::endl;
                std::cout << "  Size: " << message_size << " bytes" << std::endl;

                if (offset + message_size > packet.size()) {
                    std::cerr << "Incomplete message data" << std::endl;
                    return false;
                }

                // Parse SBE message
                bool parsed = parse_sbe_message(packet, offset, message_size);
                if (!parsed) {
                    return false;
                }

                // CRITICAL: Advance by message size (this is what real clients do)
                offset += message_size;

                std::cout << "  Advanced to offset: " << offset << std::endl;
            }

            std::cout << "Total messages processed: " << message_count << std::endl;
            return true;

        } catch (const std::exception& e) {
            std::cerr << "Exception parsing packet: " << e.what() << std::endl;
            return false;
        }
    }

    bool parse_sbe_message(const std::vector<uint8_t>& packet, size_t offset, uint16_t message_size)
    {
        if (message_size < 8) {
            std::cerr << "Message too small for SBE header" << std::endl;
            return false;
        }

        // Parse SBE header
        uint16_t block_length = packet[offset] | (packet[offset + 1] << 8);
        uint16_t template_id = packet[offset + 2] | (packet[offset + 3] << 8);
        uint16_t schema_id = packet[offset + 4] | (packet[offset + 5] << 8);
        uint16_t version = packet[offset + 6] | (packet[offset + 7] << 8);

        std::cout << "  SBE Header:" << std::endl;
        std::cout << "    Block Length: " << block_length << std::endl;
        std::cout << "    Template ID: " << template_id << std::endl;
        std::cout << "    Schema ID: " << schema_id << std::endl;
        std::cout << "    Version: " << version << std::endl;

        // Validate expected values
        if (template_id != 46) {
            std::cerr << "Unexpected template ID: " << template_id << " (expected 46)" << std::endl;
            return false;
        }

        if (block_length != 11) {
            std::cerr << "Unexpected block length: " << block_length << " (expected 11)" << std::endl;
            return false;
        }

        // Parse message content using SBE decoder
        return parse_incremental_refresh(packet, offset, message_size);
    }

    bool parse_incremental_refresh(const std::vector<uint8_t>& packet, size_t offset, uint16_t message_size)
    {
        try {
            // Create decoder and wrap for decode
            MessageHeader header;
            header.wrap(const_cast<char*>(reinterpret_cast<const char*>(packet.data())), offset, 0, packet.size());

            MDIncrementalRefreshBook46 decoder;
            decoder.wrapForDecode(
                const_cast<char*>(reinterpret_cast<const char*>(packet.data())),
                offset + header.encodedLength(),
                header.blockLength(),
                header.version(),
                packet.size());

            // Parse message fields
            uint64_t transact_time = decoder.transactTime();
            std::cout << "  Transaction Time: " << transact_time << std::endl;

            // Parse NoMDEntries group
            auto& md_entries = decoder.noMDEntries();
            std::cout << "  MD Entries: " << static_cast<int>(md_entries.count()) << std::endl;

            int entry_count = 0;
            while (md_entries.hasNext()) {
                md_entries.next();
                entry_count++;

                std::cout << "    Entry " << entry_count << ":" << std::endl;
                std::cout << "      Security ID: " << md_entries.securityID() << std::endl;
                std::cout << "      Rpt Seq: " << md_entries.rptSeq() << std::endl;
                std::cout << "      Price Level: " << static_cast<int>(md_entries.mDPriceLevel()) << std::endl;
                std::cout << "      Price: " << md_entries.mDEntryPx().mantissa() << std::endl;
                std::cout << "      Size: " << md_entries.mDEntrySize() << std::endl;
                std::cout << "      Orders: " << md_entries.numberOfOrders() << std::endl;
            }

            // Parse NoOrderIDEntries group
            auto& order_entries = decoder.noOrderIDEntries();
            std::cout << "  Order Entries: " << static_cast<int>(order_entries.count()) << std::endl;

            int order_count = 0;
            while (order_entries.hasNext()) {
                order_entries.next();
                order_count++;

                std::cout << "    Order " << order_count << ":" << std::endl;
                std::cout << "      Order ID: " << order_entries.orderID() << std::endl;
                std::cout << "      Priority: " << order_entries.mDOrderPriority() << std::endl;
                std::cout << "      Display Qty: " << order_entries.mDDisplayQty() << std::endl;
                std::cout << "      Reference ID: " << static_cast<int>(order_entries.referenceID()) << std::endl;
            }

            return true;

        } catch (const std::exception& e) {
            std::cerr << "Error parsing incremental refresh: " << e.what() << std::endl;
            return false;
        }
    }
};

void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  --ip IP          IP address to listen on (default: 239.1.1.1)\n"
              << "  --port PORT      Port to listen on (default: 20001)\n"
              << "  --verbose        Enable verbose output\n"
              << "  --help           Show this help\n\n"
              << "Example:\n"
              << "  " << program_name << " --ip 127.0.0.1 --port 20001 --verbose\n";
}

int main(int argc, char* argv[])
{
    std::string ip = "239.1.1.1"; // Default multicast IP
    uint16_t port = 20001; // Default incremental feed port
    bool verbose = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--ip") == 0 && i + 1 < argc) {
            ip = argv[++i];
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = static_cast<uint16_t>(atoi(argv[++i]));
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }

    std::cout << "CME Test Client v1.0.0" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "Listening for packets from mock server..." << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  IP: " << ip << std::endl;
    std::cout << "  Port: " << port << std::endl;
    std::cout << "  Verbose: " << (verbose ? "ON" : "OFF") << std::endl;

    CMETestClient client(verbose);

    if (!client.connect_to_server(ip, port)) {
        return 1;
    }

    client.process_packets();

    return 0;
}