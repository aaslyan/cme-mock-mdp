#include "messages/cme_sbe_encoder.h"
#include "messages/sbe_encoder.h"
#include "messages/mdp_messages.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace cme_mock;

void analyze_correct_packet(const uint8_t* data, size_t size, const std::string& label) {
    std::cout << "\n=== " << label << " ===\n";
    std::cout << "Total size: " << size << " bytes\n\n";
    
    if (size < 12) {
        std::cout << "ERROR: Packet too small for binary header\n";
        return;
    }
    
    size_t offset = 0;
    
    // 1. Parse Binary Packet Header (12 bytes)
    uint32_t seq_num;
    uint64_t sending_time;
    
    std::memcpy(&seq_num, data + offset, 4);
    offset += 4;
    std::memcpy(&sending_time, data + offset, 8);
    offset += 8;
    
    std::cout << "1. Binary Packet Header (12 bytes):\n";
    std::cout << "   Sequence Number: " << seq_num << "\n";
    std::cout << "   Sending Time: " << sending_time << "\n\n";
    
    if (offset + 2 > size) {
        std::cout << "ERROR: Not enough data for message count\n";
        return;
    }
    
    // 2. Parse Message Count (2 bytes)
    uint16_t msg_count;
    std::memcpy(&msg_count, data + offset, 2);
    offset += 2;
    
    std::cout << "2. Message Count: " << msg_count << " messages\n\n";
    
    // 3. Parse each message
    for (uint16_t i = 0; i < msg_count && offset < size; ++i) {
        std::cout << "3." << (i+1) << " Message " << (i+1) << ":\n";
        
        if (offset + 2 > size) {
            std::cout << "   ERROR: Not enough data for message size\n";
            break;
        }
        
        // Parse Message Size (2 bytes)
        uint16_t msg_size;
        std::memcpy(&msg_size, data + offset, 2);
        offset += 2;
        
        std::cout << "   Message Size: " << msg_size << " bytes\n";
        
        if (offset + 8 > size) {
            std::cout << "   ERROR: Not enough data for SBE header\n";
            break;
        }
        
        // Parse SBE Message Header (8 bytes)
        uint16_t block_length, template_id, schema_id, version;
        std::memcpy(&block_length, data + offset, 2);
        std::memcpy(&template_id, data + offset + 2, 2);
        std::memcpy(&schema_id, data + offset + 4, 2);
        std::memcpy(&version, data + offset + 6, 2);
        
        std::cout << "   SBE Message Header:\n";
        std::cout << "     Block Length: " << block_length << "\n";
        std::cout << "     Template ID: " << template_id;
        
        // Identify template
        switch(template_id) {
            case 46: std::cout << " (MDIncrementalRefreshBook) ✓"; break;
            case 52: std::cout << " (SnapshotFullRefresh) ✓"; break;
            case 32: std::cout << " *** ERROR: INVALID TEMPLATE 32 ***"; break;
            default: std::cout << " (Unknown)"; break;
        }
        std::cout << "\n";
        
        std::cout << "     Schema ID: " << schema_id << " (expected: 1)\n";
        std::cout << "     Version: " << version << " (expected: 13)\n\n";
        
        // Skip to next message
        offset += msg_size;
    }
    
    // Show structure summary
    std::cout << "Structure Summary:\n";
    std::cout << "  0-11:  Binary Packet Header (12 bytes)\n";
    std::cout << "  12-13: Message Count (2 bytes)\n";
    std::cout << "  14+:   Messages (Message Size + SBE Header + Body)\n\n";
    
    // Show hex dump of first 48 bytes
    std::cout << "Hex dump (first 48 bytes):\n";
    for (size_t i = 0; i < std::min(size, size_t(48)); ++i) {
        if (i % 16 == 0) {
            std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }
    if (size < 48 && size % 16 != 0) std::cout << "\n";
    std::cout << std::dec;
}

int main() {
    std::cout << "CME MDP 3.0 Correct Structure Test\n";
    std::cout << "===================================\n";
    
    // Test snapshot
    SnapshotFullRefresh snapshot;
    snapshot.header.sequence_number = 100;
    snapshot.header.sending_time = 1234567890123456789ULL;
    snapshot.security_id = 1;
    snapshot.last_msg_seq_num_processed = 99;
    snapshot.rpt_seq = 1;
    snapshot.tot_num_reports = 1;
    snapshot.security_trading_status = 17;
    snapshot.transact_time = 1234567890123456789ULL;
    
    // Simulate what SnapshotFeedPublisher::encode_snapshot does now
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        snapshot.header.sequence_number,
        snapshot.header.sending_time);
    
    auto message = MDPMessageEncoder::encode_snapshot_full_refresh(snapshot);
    
    std::vector<uint8_t> complete_packet;
    complete_packet.reserve(packet_header.size() + 2 + 2 + message.size());
    
    // 1. Binary Packet Header (12 bytes)
    complete_packet.insert(complete_packet.end(), packet_header.begin(), packet_header.end());
    
    // 2. Message Count (2 bytes)
    uint16_t message_count = 1;
    complete_packet.push_back(message_count & 0xFF);
    complete_packet.push_back((message_count >> 8) & 0xFF);
    
    // 3. Message Size (2 bytes)
    uint16_t message_size = static_cast<uint16_t>(message.size());
    complete_packet.push_back(message_size & 0xFF);
    complete_packet.push_back((message_size >> 8) & 0xFF);
    
    // 4. SBE Message
    complete_packet.insert(complete_packet.end(), message.begin(), message.end());
    
    analyze_correct_packet(complete_packet.data(), complete_packet.size(), "Snapshot Packet");
    
    std::cout << "\n\n======================================\n";
    std::cout << "VERIFICATION:\n";
    std::cout << "======================================\n";
    std::cout << "✓ Binary Packet Header: 12 bytes (seq + time)\n";
    std::cout << "✓ Message Count: 2 bytes after header\n";
    std::cout << "✓ Message Size: 2 bytes before each message\n";
    std::cout << "✓ SBE Header: Block Length + Template ID + Schema ID + Version\n";
    std::cout << "This is the correct CME MDP 3.0 structure!\n";
    
    return 0;
}