#include "messages/cme_sbe_encoder.h"
#include "messages/sbe_encoder.h"
#include "messages/mdp_messages.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace cme_mock;

void analyze_packet(const uint8_t* data, size_t size, const std::string& label) {
    std::cout << "\n=== " << label << " ===\n";
    std::cout << "Total size: " << size << " bytes\n\n";
    
    if (size < 14) {
        std::cout << "ERROR: Packet too small\n";
        return;
    }
    
    // Parse Binary Packet Header
    uint32_t seq_num;
    uint64_t sending_time;
    uint16_t msg_count;
    
    std::memcpy(&seq_num, data, 4);
    std::memcpy(&sending_time, data + 4, 8);
    std::memcpy(&msg_count, data + 12, 2);
    
    std::cout << "Binary Packet Header (14 bytes):\n";
    std::cout << "  Sequence: " << seq_num << "\n";
    std::cout << "  Time: " << sending_time << "\n";
    std::cout << "  Message Count: " << msg_count << "\n\n";
    
    size_t offset = 14;
    
    // Parse messages
    for (uint16_t i = 0; i < msg_count && offset < size; ++i) {
        std::cout << "Message " << (i + 1) << ":\n";
        
        if (offset + 2 > size) break;
        
        uint16_t msg_size;
        std::memcpy(&msg_size, data + offset, 2);
        std::cout << "  Size: " << msg_size << " bytes\n";
        offset += 2;
        
        if (offset + 8 <= size) {
            uint16_t block_len, template_id, schema_id, version;
            std::memcpy(&block_len, data + offset, 2);
            std::memcpy(&template_id, data + offset + 2, 2);
            std::memcpy(&schema_id, data + offset + 4, 2);
            std::memcpy(&version, data + offset + 6, 2);
            
            std::cout << "  SBE Header:\n";
            std::cout << "    Block Length: " << block_len << "\n";
            std::cout << "    Template ID: " << template_id;
            
            if (template_id == 32) {
                std::cout << " *** ERROR: INVALID TEMPLATE 32! ***";
            } else if (template_id == 46) {
                std::cout << " (MDIncrementalRefreshBook)";
            } else if (template_id == 52) {
                std::cout << " (SnapshotFullRefresh)";
            }
            std::cout << "\n";
            std::cout << "    Schema ID: " << schema_id << "\n";
            std::cout << "    Version: " << version << "\n";
            
            if (template_id == 32) {
                std::cout << "\n  *** MISALIGNMENT DETECTED! ***\n";
                std::cout << "  The gateway is reading the wrong bytes as template ID.\n";
                std::cout << "  This usually means the Binary Packet Header is wrong size.\n";
            }
        }
        
        offset += msg_size;
    }
    
    // Show hex dump of first 64 bytes
    std::cout << "\nHex dump (first 64 bytes):\n";
    for (size_t i = 0; i < std::min(size, size_t(64)); ++i) {
        if (i % 16 == 0) std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }
    if (size < 64 && size % 16 != 0) std::cout << "\n";
    std::cout << std::dec;
}

void test_snapshot() {
    std::cout << "Testing Snapshot Message\n";
    std::cout << "========================\n";
    
    SnapshotFullRefresh snapshot;
    snapshot.header.sequence_number = 100;
    snapshot.header.sending_time = 1234567890123456789ULL;
    snapshot.header.msg_count = 1;
    snapshot.security_id = 1;
    snapshot.last_msg_seq_num_processed = 99;
    snapshot.rpt_seq = 1;
    snapshot.tot_num_reports = 1;
    snapshot.security_trading_status = 17;
    snapshot.transact_time = 1234567890123456789ULL;
    
    // Build complete packet as feed_publisher does
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        snapshot.header.sequence_number,
        snapshot.header.sending_time,
        snapshot.header.msg_count);
    
    auto message = MDPMessageEncoder::encode_snapshot_full_refresh(snapshot);
    
    std::vector<uint8_t> complete_packet;
    complete_packet.reserve(packet_header.size() + 2 + message.size());
    complete_packet.insert(complete_packet.end(), packet_header.begin(), packet_header.end());
    
    uint16_t message_size = static_cast<uint16_t>(message.size());
    complete_packet.push_back(message_size & 0xFF);
    complete_packet.push_back((message_size >> 8) & 0xFF);
    
    complete_packet.insert(complete_packet.end(), message.begin(), message.end());
    
    analyze_packet(complete_packet.data(), complete_packet.size(), "Snapshot Packet");
}

void test_incremental() {
    std::cout << "\n\nTesting Incremental Message\n";
    std::cout << "===========================\n";
    
    IncrementalRefresh update;
    update.header.sequence_number = 200;
    update.header.sending_time = 9876543210987654321ULL;
    update.transact_time = 9876543210987654321ULL;
    
    MDPriceLevel level;
    level.update_action = MDUpdateAction::New;
    level.entry_type = MDEntryType::Bid;
    level.security_id = 1;
    level.rpt_seq = 1;
    level.price = 450025;
    level.quantity = 100;
    level.number_of_orders = 5;
    level.price_level = 1;
    update.price_levels.push_back(level);
    
    // Simulate what the FIXED IncrementalFeedPublisher does now
    uint16_t message_count = update.price_levels.size() + update.trades.size();
    if (message_count == 0) message_count = 1;
    
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        update.header.sequence_number,
        update.header.sending_time,
        message_count);
    
    std::vector<uint8_t> result;
    result.reserve(1024);
    result.insert(result.end(), packet_header.begin(), packet_header.end());
    
    // Process each price level
    for (const auto& lvl : update.price_levels) {
        IncrementalRefresh single_update;
        single_update.header = update.header;
        single_update.transact_time = update.transact_time;
        single_update.price_levels.push_back(lvl);
        
        auto message = MDPMessageEncoder::encode_incremental_refresh(single_update);
        
        uint16_t msg_size = static_cast<uint16_t>(message.size());
        result.push_back(msg_size & 0xFF);
        result.push_back((msg_size >> 8) & 0xFF);
        
        result.insert(result.end(), message.begin(), message.end());
    }
    
    analyze_packet(result.data(), result.size(), "Incremental Packet");
}

int main() {
    std::cout << "CME MDP 3.0 Alignment Fix Verification\n";
    std::cout << "======================================\n";
    
    test_snapshot();
    test_incremental();
    
    std::cout << "\n\n======================================\n";
    std::cout << "SUMMARY:\n";
    std::cout << "======================================\n";
    std::cout << "If Template ID shows as 32, we still have alignment issues.\n";
    std::cout << "If Template ID shows as 52 (snapshot) or 46 (incremental), the fix worked!\n";
    
    return 0;
}