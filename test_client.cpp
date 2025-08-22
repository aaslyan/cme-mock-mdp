#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace cme_sbe;

// Simple hexdump function
void hexdump(const std::vector<uint8_t>& data, const std::string& title = "Data")
{
    std::cout << "\n"
              << title << " (" << data.size() << " bytes):" << std::endl;
    for (size_t i = 0; i < data.size(); i += 16) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << i << ": ";

        for (size_t j = 0; j < 16; ++j) {
            if (i + j < data.size()) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)data[i + j] << " ";
            } else {
                std::cout << "   ";
            }
        }

        std::cout << " |";
        for (size_t j = 0; j < 16 && i + j < data.size(); ++j) {
            char c = data[i + j];
            std::cout << (isprint(c) ? c : '.');
        }
        std::cout << "|" << std::dec << std::endl;
    }
    std::cout << std::endl;
}

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

    // Public interface for parsing CME data from external sources
    static bool parse_packet(const uint8_t* data, size_t length, bool verbose_output = false)
    {
        CMETestClient parser(verbose_output);
        return parser.parse_cme_data(data, length);
    }

    // Debug function specifically for analyzing actual CME data to identify mock server issues
    static std::string analyze_for_mock_server_debug(const uint8_t* data, size_t length)
    {
        std::string analysis = "=== CME DATA ANALYSIS FOR MOCK SERVER DEBUG ===\n";
        analysis += "Packet size: " + std::to_string(length) + " bytes\n\n";

        if (length < 12) {
            return analysis + "ERROR: Packet too small for CME header\n";
        }

        try {
            size_t offset = 0;

            // Parse packet header
            uint32_t sequence = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
            uint64_t timestamp = 0;
            for (int i = 0; i < 8; ++i) {
                timestamp |= (static_cast<uint64_t>(data[4 + i]) << (i * 8));
            }

            analysis += "Packet Header:\n";
            analysis += "  Sequence: " + std::to_string(sequence) + "\n";
            analysis += "  Timestamp: " + std::to_string(timestamp) + "\n\n";

            offset = 12;
            int msg_count = 0;
            bool has_order_entries = false;
            bool proper_alignment = true;

            while (offset + 2 <= length) {
                uint16_t msg_size = data[offset] | (data[offset + 1] << 8);
                offset += 2;

                if (msg_size == 0)
                    break; // End of messages
                if (offset + msg_size > length)
                    break;

                msg_count++;
                analysis += "Message " + std::to_string(msg_count) + ":\n";
                analysis += "  Size: " + std::to_string(msg_size) + " bytes\n";

                // Check alignment
                if (msg_size % 8 != 0) {
                    proper_alignment = false;
                    analysis += "  ❌ NOT 8-byte aligned\n";
                } else {
                    analysis += "  ✅ 8-byte aligned\n";
                }

                // Parse SBE header if possible
                if (msg_size >= 8) {
                    try {
                        MessageHeader header;
                        header.wrap(const_cast<char*>(reinterpret_cast<const char*>(data)),
                            offset, 0, length);

                        analysis += "  Template ID: " + std::to_string(header.templateId()) + "\n";
                        analysis += "  Block Length: " + std::to_string(header.blockLength()) + "\n";

                        // Check for OrderID entries if Template 46
                        if (header.templateId() == 46) {
                            try {
                                MDIncrementalRefreshBook46 decoder;
                                decoder.wrapForDecode(
                                    const_cast<char*>(reinterpret_cast<const char*>(data)),
                                    offset + header.encodedLength(),
                                    header.blockLength(),
                                    header.version(),
                                    length);

                                auto& md_entries = decoder.noMDEntries();
                                auto& order_entries = decoder.noOrderIDEntries();

                                analysis += "  MD Entries: " + std::to_string(md_entries.count()) + "\n";
                                analysis += "  Order Entries: " + std::to_string(order_entries.count()) + "\n";

                                if (order_entries.count() > 0) {
                                    has_order_entries = true;
                                    analysis += "  ✅ HAS OrderID entries (production format)\n";
                                } else {
                                    analysis += "  ❌ Missing OrderID entries\n";
                                }

                            } catch (...) {
                                analysis += "  Error parsing message content\n";
                            }
                        }

                    } catch (...) {
                        analysis += "  Error parsing SBE header\n";
                    }
                }

                offset += msg_size;
                analysis += "\n";
            }

            // Summary for mock server debugging
            analysis += "=== MOCK SERVER DEBUG SUMMARY ===\n";
            analysis += "Total messages: " + std::to_string(msg_count) + "\n";

            if (has_order_entries) {
                analysis += "✅ Real CME data includes OrderID entries\n";
                analysis += "   → Mock server MUST implement NoOrderIDEntries group\n";
            } else {
                analysis += "❌ No OrderID entries found\n";
                analysis += "   → Check if this is incomplete data\n";
            }

            if (proper_alignment) {
                analysis += "✅ All messages are 8-byte aligned\n";
                analysis += "   → Mock server alignment implementation is correct\n";
            } else {
                analysis += "❌ Some messages not 8-byte aligned\n";
                analysis += "   → Fix mock server alignment calculation\n";
            }

            analysis += "\nACTION ITEMS for mock server:\n";
            analysis += "1. Ensure OrderID entries are populated with realistic data\n";
            analysis += "2. Verify 8-byte message alignment: ((length + 7) / 8) * 8\n";
            analysis += "3. Match exact message sizes from this real data\n";
            analysis += "4. Include proper MD entries with price levels\n";

        } catch (const std::exception& e) {
            analysis += "Analysis error: " + std::string(e.what()) + "\n";
        }

        return analysis;
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

    // Main function to parse CME packet data from pointer and length
    bool parse_cme_data(const uint8_t* data, size_t length)
    {
        if (!data || length == 0) {
            std::cerr << "Invalid data pointer or length" << std::endl;
            return false;
        }

        if (verbose) {
            std::vector<uint8_t> packet_data(data, data + length);
            hexdump(packet_data, "Raw Packet Data");
        }

        try {
            size_t offset = 0;

            // Step 1: Parse packet header (12 bytes)
            if (length < 12) {
                std::cerr << "Packet too small for header" << std::endl;
                return false;
            }

            uint32_t sequence_number = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

            uint64_t sending_time = 0;
            for (int i = 0; i < 8; ++i) {
                sending_time |= (static_cast<uint64_t>(data[4 + i]) << (i * 8));
            }

            std::cout << "Packet Header:" << std::endl;
            std::cout << "  Sequence: " << sequence_number << std::endl;
            std::cout << "  Timestamp: " << sending_time << std::endl;

            offset = 12;
            int message_count = 0;

            // Step 2: Parse messages
            while (offset < length) {
                if (offset + 2 > length) {
                    std::cerr << "Incomplete message size field" << std::endl;
                    return false;
                }

                // Read message size
                uint16_t message_size = data[offset] | (data[offset + 1] << 8);
                offset += 2;

                message_count++;
                std::cout << "\nMessage " << message_count << ":" << std::endl;
                std::cout << "  Size: " << message_size << " bytes" << std::endl;

                if (offset + message_size > length) {
                    std::cerr << "Incomplete message data" << std::endl;
                    return false;
                }

                // Parse SBE message
                bool parsed = parse_sbe_message_from_buffer(data, offset, message_size, length);
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

            // Use the new parse function with pointer and length
            bool parsed = parse_cme_data(buffer, static_cast<size_t>(bytes_received));

            if (parsed) {
                std::cout << "✅ Packet parsed successfully" << std::endl;
            } else {
                std::cout << "❌ Packet parsing failed" << std::endl;
            }

            std::cout << "========================================\n"
                      << std::endl;
        }
    }

    // SBE message parsing function that works with pointer and length
    bool parse_sbe_message_from_buffer(const uint8_t* data, size_t offset, uint16_t message_size, size_t total_length)
    {
        if (message_size < 8) {
            std::cerr << "Message too small for SBE header" << std::endl;
            return false;
        }

        try {
            // Use SBE MessageHeader to properly decode the header
            MessageHeader header;
            header.wrap(const_cast<char*>(reinterpret_cast<const char*>(data)),
                offset, 0, total_length);

            uint16_t block_length = header.blockLength();
            uint16_t template_id = header.templateId();
            uint16_t schema_id = header.schemaId();
            uint16_t version = header.version();

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
            return parse_incremental_refresh_from_buffer(header, data, offset, message_size, total_length);

        } catch (const std::exception& e) {
            std::cerr << "Error parsing SBE header: " << e.what() << std::endl;
            return false;
        }
    }

    bool parse_incremental_refresh_from_buffer(const MessageHeader& header, const uint8_t* data, size_t offset, uint16_t message_size, size_t total_length)
    {
        try {
            // Create decoder and wrap for decode using the already parsed header
            MDIncrementalRefreshBook46 decoder;
            decoder.wrapForDecode(
                const_cast<char*>(reinterpret_cast<const char*>(data)),
                offset + header.encodedLength(),
                header.blockLength(),
                header.version(),
                total_length);

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

        try {
            // Use SBE MessageHeader to properly decode the header
            MessageHeader header;
            header.wrap(const_cast<char*>(reinterpret_cast<const char*>(packet.data())),
                offset, 0, packet.size());

            uint16_t block_length = header.blockLength();
            uint16_t template_id = header.templateId();
            uint16_t schema_id = header.schemaId();
            uint16_t version = header.version();

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
            return parse_incremental_refresh(header, packet, offset, message_size);

        } catch (const std::exception& e) {
            std::cerr << "Error parsing SBE header: " << e.what() << std::endl;
            return false;
        }
    }

    bool parse_incremental_refresh(const MessageHeader& header, const std::vector<uint8_t>& packet, size_t offset, uint16_t message_size)
    {
        try {
            // Create decoder and wrap for decode using the already parsed header
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