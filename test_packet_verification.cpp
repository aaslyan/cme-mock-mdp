#include "utils/packet_verifier.h"
#include "utils/hexdump.h"
#include "messages/cme_sbe_encoder.h"
#include <iostream>
#include <cstring>

using namespace cme_mock;

int main(int argc, char* argv[])
{
    bool verbose = false;
    
    // Check for --verbose flag
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
            break;
        }
    }
    
    std::cout << "=== Testing Packet Verification " << (verbose ? "(VERBOSE MODE)" : "") << " ===" << std::endl;
    
    // Test 1: Create a single 88-byte message packet
    IncrementalRefresh refresh;
    refresh.transact_time = 1234567890123456789ULL;
    
    MDPriceLevel level;
    level.security_id = 100;
    level.rpt_seq = 1;
    level.price = 150000;
    level.quantity = 10;
    level.number_of_orders = 1;
    level.price_level = 1;
    level.entry_type = MDEntryType::Bid;
    level.update_action = MDUpdateAction::New;
    refresh.price_levels.push_back(level);
    
    // Generate SBE message
    auto sbe_message = CMESBEEncoder::encode_incremental_refresh(refresh);
    
    if (verbose) {
        std::cout << "\n--- SBE Message Details ---" << std::endl;
        std::cout << "SBE message size: " << sbe_message.size() << " bytes" << std::endl;
        hexdump(sbe_message, "SBE Message Content");
    }
    
    // Create complete packet
    std::vector<std::vector<uint8_t>> messages = { sbe_message };
    auto packet = CMESBEEncoder::encode_multi_message_packet(12345, 987654321ULL, messages);
    
    if (verbose) {
        std::cout << "\n--- Complete Packet Details ---" << std::endl;
        std::cout << "Complete packet size: " << packet.size() << " bytes" << std::endl;
        hexdump(packet, "Complete Packet");
        
        // Show packet structure breakdown
        std::cout << "\n--- Packet Structure Breakdown ---" << std::endl;
        std::cout << "Bytes 0-11: Packet header" << std::endl;
        std::cout << "Bytes 12-13: Message size field" << std::endl;
        std::cout << "Bytes 14+: SBE message content" << std::endl;
        
        // Extract and show message size field
        uint16_t msg_size = packet[12] | (packet[13] << 8);
        std::cout << "Message size field value: " << msg_size << " bytes" << std::endl;
        std::cout << "Expected size field: " << sbe_message.size() << " bytes" << std::endl;
        if (msg_size == sbe_message.size()) {
            std::cout << "✅ Message size field is correct" << std::endl;
        } else {
            std::cout << "❌ Message size field mismatch!" << std::endl;
        }
    }
    
    // Test the verification
    bool is_valid = PacketVerifier::verify_and_log(packet, "Single 88-byte message packet");
    
    if (verbose && is_valid) {
        std::cout << "\n--- Client Processing Simulation ---" << std::endl;
        std::cout << "How a client would process this packet:" << std::endl;
        std::cout << "1. Read packet header (12 bytes)" << std::endl;
        std::cout << "2. Read message size: " << (packet[12] | (packet[13] << 8)) << " bytes" << std::endl;
        std::cout << "3. Read " << (packet[12] | (packet[13] << 8)) << " bytes of message data" << std::endl;
        std::cout << "4. Advance pointer by " << (packet[12] | (packet[13] << 8)) << " bytes" << std::endl;
        std::cout << "5. Reached end of packet ✅" << std::endl;
    }
    
    // Test 2: Create a multi-message packet (like real CME 132-byte format)
    if (verbose) {
        std::cout << "\n\n=== Testing Multi-Message Packet ===\n" << std::endl;
        
        // Create a second, smaller message (0 entries = ~30 bytes)
        IncrementalRefresh small_refresh;
        small_refresh.transact_time = 1234567890123456789ULL;
        // No price levels = smaller message
        
        auto small_message = CMESBEEncoder::encode_incremental_refresh(small_refresh);
        std::cout << "Small message size: " << small_message.size() << " bytes" << std::endl;
        
        // Create multi-message packet
        std::vector<std::vector<uint8_t>> multi_messages = { sbe_message, small_message };
        auto multi_packet = CMESBEEncoder::encode_multi_message_packet(12346, 987654322ULL, multi_messages);
        
        std::cout << "Multi-message packet size: " << multi_packet.size() << " bytes" << std::endl;
        hexdump(multi_packet, "Multi-Message Packet");
        
        PacketVerifier::verify_and_log(multi_packet, "Multi-message packet (like CME 132-byte)");
    }
    
    return is_valid ? 0 : 1;
}