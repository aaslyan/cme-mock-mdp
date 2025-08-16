#include <iostream>
#include <iomanip>
#include <vector>
#include "messages/sbe_encoder.h"
#include "messages/mdp_messages.h"

using namespace cme_mock;

int main() {
    // Create a simple incremental refresh message
    IncrementalRefresh update;
    update.header.sequence_number = 0x047c;  // 1148
    update.header.sending_time = 0x185c2456d28eed00;
    update.header.msg_count = 1;  // This field shouldn't be in the packet!
    update.transact_time = 0x185c2456d28b2600;
    
    // Add one price level
    MDPriceLevel level;
    level.security_id = 0x0f;
    level.rpt_seq = 0x2452;
    level.entry_type = MDEntryType::Bid;
    level.price = 225500;
    level.quantity = 600;
    level.number_of_orders = 1;
    level.price_level = 1;
    level.update_action = MDUpdateAction::New;
    
    update.price_levels.push_back(level);
    
    // Encode using the same logic as feed_publisher
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        update.header.sequence_number,
        update.header.sending_time);
    
    auto message = MDPMessageEncoder::encode_incremental_refresh(update);
    
    std::cout << "Packet header size: " << packet_header.size() << " bytes" << std::endl;
    std::cout << "Message size: " << message.size() << " bytes" << std::endl;
    
    // Build complete packet
    std::vector<uint8_t> result;
    result.reserve(packet_header.size() + 2 + message.size());
    
    // Add packet header
    result.insert(result.end(), packet_header.begin(), packet_header.end());
    
    // Add message size (uint16_t, little-endian)
    uint16_t message_size = static_cast<uint16_t>(message.size());
    result.push_back(message_size & 0xFF);
    result.push_back((message_size >> 8) & 0xFF);
    
    // Add SBE message
    result.insert(result.end(), message.begin(), message.end());
    
    std::cout << "\nComplete packet (" << result.size() << " bytes):" << std::endl;
    std::cout << "Offset 0-11 (Packet Header): ";
    for (int i = 0; i < 12 && i < result.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)result[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Offset 12-13 (Message Size): ";
    for (int i = 12; i < 14 && i < result.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)result[i] << " ";
    }
    std::cout << " = " << std::dec << message_size << " bytes" << std::endl;
    
    std::cout << "Offset 14-21 (SBE Header): ";
    for (int i = 14; i < 22 && i < result.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)result[i] << " ";
    }
    std::cout << std::endl;
    
    // Decode SBE header
    if (result.size() >= 22) {
        uint16_t block_len = result[14] | (result[15] << 8);
        uint16_t template_id = result[16] | (result[17] << 8);
        uint16_t schema_id = result[18] | (result[19] << 8);
        uint16_t version = result[20] | (result[21] << 8);
        
        std::cout << "\nSBE Header decoded:" << std::endl;
        std::cout << "  Block Length: " << block_len << std::endl;
        std::cout << "  Template ID: " << template_id << std::endl;
        std::cout << "  Schema ID: " << schema_id << std::endl;
        std::cout << "  Version: " << version << std::endl;
    }
    
    return 0;
}