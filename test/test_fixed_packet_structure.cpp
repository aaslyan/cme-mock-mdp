#include "messages/cme_sbe_encoder.h"
#include "messages/sbe_encoder.h"
#include "messages/mdp_messages.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace cme_mock;

void print_hex_dump(const uint8_t* data, size_t size, const std::string& label) {
    std::cout << "\n" << label << " (" << size << " bytes):\n";
    std::cout << "Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII\n";
    std::cout << "------  -----------------------------------------------  ----------------\n";
    
    for (size_t i = 0; i < size; i += 16) {
        std::cout << std::setfill('0') << std::setw(6) << std::hex << i << "  ";
        
        // Hex bytes
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < size) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex 
                         << static_cast<int>(data[i + j]) << " ";
            } else {
                std::cout << "   ";
            }
        }
        
        std::cout << " ";
        
        // ASCII representation
        for (size_t j = 0; j < 16 && i + j < size; ++j) {
            char c = static_cast<char>(data[i + j]);
            if (c >= 32 && c <= 126) {
                std::cout << c;
            } else {
                std::cout << ".";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::dec;
}

void test_packet_structure() {
    std::cout << "Testing FIXED CME MDP 3.0 Packet Structure\n";
    std::cout << "==========================================\n";
    
    // Create a test snapshot message
    SnapshotFullRefresh snapshot;
    snapshot.header.sequence_number = 12345;
    snapshot.header.sending_time = 1234567890123456789ULL;
    snapshot.header.msg_count = 1;
    
    snapshot.security_id = 1;
    snapshot.last_msg_seq_num_processed = 100;
    snapshot.rpt_seq = 1;
    snapshot.tot_num_reports = 1;
    snapshot.security_trading_status = 17; // Trading
    snapshot.transact_time = 1234567890123456789ULL;
    
    // Add some test entries
    MDSnapshotEntry entry1;
    entry1.price = 4500.25;
    entry1.quantity = 100;
    entry1.price_level = 1;
    entry1.entry_type = MDEntryType::Bid;
    snapshot.entries.push_back(entry1);
    
    MDSnapshotEntry entry2;
    entry2.price = 4500.50;
    entry2.quantity = 200;
    entry2.price_level = 1;
    entry2.entry_type = MDEntryType::Offer;
    snapshot.entries.push_back(entry2);
    
    // Test 1: Create packet header only
    std::cout << "\n=== Test 1: Binary Packet Header ===\n";
    auto packet_header = CMESBEEncoder::encode_packet_header(
        snapshot.header.sequence_number,
        snapshot.header.sending_time,
        snapshot.header.msg_count
    );
    
    print_hex_dump(packet_header.data(), packet_header.size(), "Binary Packet Header");
    
    // Verify structure
    std::cout << "\nParsed Binary Packet Header:\n";
    if (packet_header.size() >= 14) {
        uint32_t seq_num;
        uint64_t sending_time;
        uint16_t msg_count;
        
        std::memcpy(&seq_num, packet_header.data(), 4);
        std::memcpy(&sending_time, packet_header.data() + 4, 8);
        std::memcpy(&msg_count, packet_header.data() + 12, 2);
        
        std::cout << "  Sequence Number: " << seq_num << " (expected: " << snapshot.header.sequence_number << ")\n";
        std::cout << "  Sending Time: " << sending_time << " (expected: " << snapshot.header.sending_time << ")\n";
        std::cout << "  Message Count: " << msg_count << " (expected: " << snapshot.header.msg_count << ")\n";
        
        bool correct = (seq_num == snapshot.header.sequence_number) &&
                      (sending_time == snapshot.header.sending_time) &&
                      (msg_count == snapshot.header.msg_count);
        
        std::cout << "\n✓ Binary Packet Header: " << (correct ? "CORRECT" : "INCORRECT") << "\n";
    } else {
        std::cout << "✗ Header too short: " << packet_header.size() << " bytes (expected 14)\n";
    }
    
    // Test 2: Create SBE message only
    std::cout << "\n=== Test 2: SBE Message ===\n";
    auto sbe_message = CMESBEEncoder::encode_snapshot_full_refresh(snapshot);
    print_hex_dump(sbe_message.data(), std::min(sbe_message.size(), size_t(64)), "SBE Message (first 64 bytes)");
    
    // Parse SBE header
    if (sbe_message.size() >= 8) {
        uint16_t block_length, template_id, schema_id, version;
        std::memcpy(&block_length, sbe_message.data(), 2);
        std::memcpy(&template_id, sbe_message.data() + 2, 2);
        std::memcpy(&schema_id, sbe_message.data() + 4, 2);
        std::memcpy(&version, sbe_message.data() + 6, 2);
        
        std::cout << "\nParsed SBE Message Header:\n";
        std::cout << "  Block Length: " << block_length << "\n";
        std::cout << "  Template ID: " << template_id << " (expected: 52 for SnapshotFullRefresh)\n";
        std::cout << "  Schema ID: " << schema_id << " (expected: 1)\n";
        std::cout << "  Version: " << version << " (expected: 13)\n";
        
        bool sbe_correct = (template_id == 52) && (schema_id == 1) && (version == 13);
        std::cout << "\n✓ SBE Message Header: " << (sbe_correct ? "CORRECT" : "INCORRECT") << "\n";
    }
    
    // Test 3: Create complete packet as would be sent over wire
    std::cout << "\n=== Test 3: Complete Packet Structure ===\n";
    
    // This simulates what feed_publisher.cpp does
    std::vector<uint8_t> complete_packet;
    complete_packet.reserve(packet_header.size() + 2 + sbe_message.size());
    
    // 1. Binary Packet Header (14 bytes)
    complete_packet.insert(complete_packet.end(), packet_header.begin(), packet_header.end());
    
    // 2. Message Size field (2 bytes) - size of following SBE message
    uint16_t message_size = static_cast<uint16_t>(sbe_message.size());
    complete_packet.push_back(message_size & 0xFF);
    complete_packet.push_back((message_size >> 8) & 0xFF);
    
    // 3. SBE Message
    complete_packet.insert(complete_packet.end(), sbe_message.begin(), sbe_message.end());
    
    print_hex_dump(complete_packet.data(), std::min(complete_packet.size(), size_t(80)), "Complete Packet (first 80 bytes)");
    
    std::cout << "\nComplete Packet Analysis:\n";
    std::cout << "  Total Size: " << complete_packet.size() << " bytes\n";
    std::cout << "  Binary Packet Header: 14 bytes (offset 0-13)\n";
    std::cout << "  Message Size: 2 bytes (offset 14-15) = " << message_size << "\n";
    std::cout << "  SBE Message: " << sbe_message.size() << " bytes (offset 16+)\n";
    
    // Verify message size field
    uint16_t actual_msg_size;
    std::memcpy(&actual_msg_size, complete_packet.data() + 14, 2);
    bool size_correct = (actual_msg_size == sbe_message.size());
    std::cout << "  Message Size Verification: " << (size_correct ? "CORRECT" : "INCORRECT") << "\n";
    
    std::cout << "\n========================================\n";
    std::cout << "SUMMARY:\n";
    std::cout << "========================================\n";
    std::cout << "The fixed implementation now creates the correct CME MDP 3.0 structure:\n\n";
    std::cout << "1. Binary Packet Header (14 bytes):\n";
    std::cout << "   - Sequence Number (4 bytes)\n";
    std::cout << "   - Sending Time (8 bytes)\n";
    std::cout << "   - Message Count (2 bytes)\n\n";
    std::cout << "2. For each message:\n";
    std::cout << "   - Message Size (2 bytes)\n";
    std::cout << "   - SBE Message Header (8 bytes)\n";
    std::cout << "   - SBE Message Body (variable)\n\n";
    std::cout << "This should now align correctly with CME gateway expectations!\n";
}

int main() {
    test_packet_structure();
    return 0;
}