#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include "messages/sbe_encoder.h"
#include "messages/mdp_messages.h"
#include "network/feed_publisher.h"

using namespace cme_mock;

uint64_t get_timestamp_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void analyze_packet(const std::vector<uint8_t>& packet) {
    std::cout << "\nPacket size: " << packet.size() << " bytes" << std::endl;
    
    if (packet.size() < 12) {
        std::cout << "Packet too small for header!" << std::endl;
        return;
    }
    
    // Parse packet header
    uint32_t seq = packet[0] | (packet[1] << 8) | (packet[2] << 16) | (packet[3] << 24);
    std::cout << "Sequence: " << seq << std::endl;
    
    size_t offset = 12; // After packet header
    int msg_count = 0;
    
    std::cout << "\nMessages in packet:" << std::endl;
    
    while (offset + 2 <= packet.size()) {
        // Read message size
        uint16_t msgsize = packet[offset] | (packet[offset + 1] << 8);
        std::cout << "\n  Message " << ++msg_count << ":" << std::endl;
        std::cout << "    Offset: " << offset << std::endl;
        std::cout << "    Size field: " << msgsize << " bytes" << std::endl;
        
        offset += 2; // Skip message size field
        
        if (offset + msgsize > packet.size()) {
            std::cout << "    ERROR: Message extends beyond packet!" << std::endl;
            break;
        }
        
        if (msgsize >= 8) {
            // Parse SBE header
            uint16_t block_len = packet[offset] | (packet[offset + 1] << 8);
            uint16_t template_id = packet[offset + 2] | (packet[offset + 3] << 8);
            uint16_t schema_id = packet[offset + 4] | (packet[offset + 5] << 8);
            uint16_t version = packet[offset + 6] | (packet[offset + 7] << 8);
            
            std::cout << "    SBE Header: BlockLen=" << block_len 
                      << " Template=" << template_id 
                      << " Schema=" << schema_id 
                      << " Version=" << version << std::endl;
        }
        
        // Move to next message
        offset += msgsize;
    }
    
    std::cout << "\nTotal messages found: " << msg_count << std::endl;
    std::cout << "Remaining bytes: " << (packet.size() - offset) << std::endl;
}

int main() {
    std::cout << "Testing Multi-Message Packet Structure" << std::endl;
    std::cout << "======================================" << std::endl;
    
    // Create incremental refresh with multiple updates
    IncrementalRefresh update;
    update.header.sequence_number = 100;
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 3; // We'll have 3 messages
    update.transact_time = get_timestamp_ns();
    
    // Add 2 price levels
    for (int i = 0; i < 2; ++i) {
        MDPriceLevel level;
        level.security_id = 31 + i; // FX instruments
        level.rpt_seq = 1000 + i;
        level.entry_type = (i == 0) ? MDEntryType::Bid : MDEntryType::Offer;
        level.price = 108500 + (i * 10);
        level.quantity = 1000000 * (i + 1);
        level.number_of_orders = 1;
        level.price_level = i + 1;
        level.update_action = MDUpdateAction::New;
        
        update.price_levels.push_back(level);
    }
    
    // Add 1 trade
    MDTrade trade;
    trade.security_id = 33;
    trade.rpt_seq = 2000;
    trade.price = 108510;
    trade.quantity = 500000;
    trade.number_of_orders = 1;
    trade.aggressor_side = 'B';
    // trade_id field not available in MDTrade struct
    
    update.trades.push_back(trade);
    
    // Simulate the encoding using IncrementalFeedPublisher's logic
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        update.header.sequence_number,
        update.header.sending_time);
    
    std::vector<uint8_t> result;
    result.reserve(1024);
    
    // Add packet header
    result.insert(result.end(), packet_header.begin(), packet_header.end());
    
    // Process each price level as a separate message
    for (const auto& level : update.price_levels) {
        IncrementalRefresh single_update;
        single_update.header = update.header;
        single_update.transact_time = update.transact_time;
        single_update.price_levels.push_back(level);
        
        auto message = MDPMessageEncoder::encode_incremental_refresh(single_update);
        
        // Add message size
        uint16_t message_size = static_cast<uint16_t>(message.size());
        result.push_back(message_size & 0xFF);
        result.push_back((message_size >> 8) & 0xFF);
        
        // Add SBE message
        result.insert(result.end(), message.begin(), message.end());
    }
    
    // Process trade as a separate message
    for (const auto& t : update.trades) {
        IncrementalRefresh single_update;
        single_update.header = update.header;
        single_update.transact_time = update.transact_time;
        single_update.trades.push_back(t);
        
        auto message = MDPMessageEncoder::encode_incremental_refresh(single_update);
        
        // Add message size
        uint16_t message_size = static_cast<uint16_t>(message.size());
        result.push_back(message_size & 0xFF);
        result.push_back((message_size >> 8) & 0xFF);
        
        // Add SBE message
        result.insert(result.end(), message.begin(), message.end());
    }
    
    // Analyze the packet
    analyze_packet(result);
    
    // Print hex dump of first 128 bytes
    std::cout << "\nHex dump (first 128 bytes):" << std::endl;
    for (size_t i = 0; i < std::min(result.size(), size_t(128)); ++i) {
        if (i % 16 == 0) std::cout << std::endl << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)result[i] << " ";
    }
    std::cout << std::dec << std::endl;
    
    return 0;
}