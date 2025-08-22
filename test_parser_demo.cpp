#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace cme_sbe;

// Simple hexdump function
void hexdump(const uint8_t* data, size_t length, const std::string& title = "Data") {
    std::cout << "\n" << title << " (" << length << " bytes):" << std::endl;
    for (size_t i = 0; i < length; i += 16) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << i << ": ";
        
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < length) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)data[i + j] << " ";
            } else {
                std::cout << "   ";
            }
        }
        
        std::cout << " |";
        for (size_t j = 0; j < 16 && i + j < length; ++j) {
            char c = data[i + j];
            std::cout << (isprint(c) ? c : '.');
        }
        std::cout << "|" << std::dec << std::endl;
    }
    std::cout << std::endl;
}

// CME packet parser function (from test_client.cpp)
bool parse_cme_data(const uint8_t* data, size_t length, bool verbose = true)
{
    if (!data || length == 0) {
        std::cerr << "Invalid data pointer or length" << std::endl;
        return false;
    }

    if (verbose) {
        hexdump(data, length, "Raw Packet Data");
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

            // Parse SBE message using proper SBE API
            try {
                MessageHeader header;
                header.wrap(const_cast<char*>(reinterpret_cast<const char*>(data)), 
                           offset, 0, length);

                std::cout << "  SBE Header:" << std::endl;
                std::cout << "    Block Length: " << header.blockLength() << std::endl;
                std::cout << "    Template ID: " << header.templateId() << std::endl;
                std::cout << "    Schema ID: " << header.schemaId() << std::endl;
                std::cout << "    Version: " << header.version() << std::endl;

                // Validate expected values
                if (header.templateId() == 46 && header.blockLength() == 11) {
                    std::cout << "  ✅ Valid CME MDP 3.0 format detected" << std::endl;
                } else {
                    std::cout << "  ⚠️  Unexpected message format" << std::endl;
                }

            } catch (const std::exception& e) {
                std::cerr << "  Error parsing SBE header: " << e.what() << std::endl;
            }

            // CRITICAL: Advance by message size (this is what real clients do)
            offset += message_size;
            std::cout << "  Advanced to offset: " << offset << std::endl;
        }

        std::cout << "\nTotal messages processed: " << message_count << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Exception parsing packet: " << e.what() << std::endl;
        return false;
    }
}

int main()
{
    std::cout << "CME Packet Parser Demo" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "\nDemonstrating pointer/length parsing interface...\n" << std::endl;

    // Example 1: Create a sample packet (this would normally come from network)
    std::vector<uint8_t> sample_packet = {
        // Packet header (12 bytes)
        0x01, 0x00, 0x00, 0x00,  // sequence_number = 1
        0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,  // timestamp
        
        // Message 1 size (2 bytes)
        0x58, 0x00,  // message_size = 88 bytes
        
        // SBE Message Header (8 bytes)
        0x0B, 0x00,  // block_length = 11
        0x2E, 0x00,  // template_id = 46
        0x01, 0x00,  // schema_id = 1
        0x0D, 0x00,  // version = 13
        
        // Message body (80 bytes of padding to reach 88 total)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // transactTime
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // Demonstrate parsing with pointer and length
    bool result = parse_cme_data(sample_packet.data(), sample_packet.size(), true);
    
    if (result) {
        std::cout << "\n✅ Packet parsing successful!" << std::endl;
    } else {
        std::cout << "\n❌ Packet parsing failed!" << std::endl;
    }

    std::cout << "\nParser interface summary:" << std::endl;
    std::cout << "- Function: parse_cme_data(const uint8_t* data, size_t length, bool verbose)" << std::endl;
    std::cout << "- Uses proper SBE API for message headers" << std::endl;
    std::cout << "- Handles CME binary protocol fields correctly" << std::endl;
    std::cout << "- Validates CME MDP 3.0 format (Template ID 46, Block Length 11)" << std::endl;

    return 0;
}