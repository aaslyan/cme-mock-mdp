#include "messages/cme_sbe_encoder.h"
#include "messages/mdp_messages.h"
#include <iostream>
#include <iomanip>

using namespace cme_mock;

void printHex(const std::vector<uint8_t>& data, const std::string& label) {
    std::cout << label << " (" << data.size() << " bytes):\n";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i % 16 == 0) std::cout << std::setw(4) << std::setfill('0') << i << ": ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)data[i] << " ";
        if ((i + 1) % 16 == 0 || i == data.size() - 1) std::cout << "\n";
    }
    std::cout << std::dec << std::endl;
}

void analyzePacketStructure() {
    std::cout << "Analyzing Complete Packet Structure" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Create a simple incremental message
    IncrementalRefresh inc;
    inc.transact_time = 123456789;
    
    MDPriceLevel level;
    level.update_action = MDUpdateAction::New;
    level.entry_type = MDEntryType::Bid;
    level.security_id = 31;
    level.rpt_seq = 1;
    level.price = 10000;
    level.quantity = 100;
    level.number_of_orders = 1;
    level.price_level = 1;
    inc.price_levels.push_back(level);
    
    // Encode packet header
    auto packet_header = CMESBEEncoder::encode_packet_header(12345, 987654321);
    printHex(packet_header, "Packet Header");
    
    // Encode SBE message
    auto sbe_message = CMESBEEncoder::encode_incremental_refresh(inc);
    printHex(sbe_message, "SBE Message");
    
    // Create complete packet (like in feed_publisher.cpp)
    std::vector<uint8_t> complete_packet;
    complete_packet.reserve(packet_header.size() + 2 + sbe_message.size());
    
    // Add packet header
    complete_packet.insert(complete_packet.end(), packet_header.begin(), packet_header.end());
    
    // Add message size field (uint16, little-endian)
    uint16_t message_size = static_cast<uint16_t>(sbe_message.size());
    complete_packet.push_back(message_size & 0xFF);
    complete_packet.push_back((message_size >> 8) & 0xFF);
    
    // Add SBE message
    complete_packet.insert(complete_packet.end(), sbe_message.begin(), sbe_message.end());
    
    printHex(complete_packet, "Complete Packet");
    
    // Analyze structure
    std::cout << "\nPacket Structure Analysis:" << std::endl;
    std::cout << "- Packet Header: " << packet_header.size() << " bytes (offset 0-" << (packet_header.size()-1) << ")" << std::endl;
    std::cout << "- Message Size: 2 bytes (offset " << packet_header.size() << "-" << (packet_header.size()+1) << ")" << std::endl;
    std::cout << "- SBE Message: " << sbe_message.size() << " bytes (offset " << (packet_header.size()+2) << "-" << (complete_packet.size()-1) << ")" << std::endl;
    std::cout << "- Total packet: " << complete_packet.size() << " bytes" << std::endl;
    
    // Check message size field
    uint16_t parsed_message_size = complete_packet[packet_header.size()] | 
                                  (complete_packet[packet_header.size()+1] << 8);
    std::cout << "\nMessage size field value: " << parsed_message_size << std::endl;
    std::cout << "Actual SBE message size: " << sbe_message.size() << std::endl;
    std::cout << "Match: " << (parsed_message_size == sbe_message.size() ? "YES" : "NO") << std::endl;
    
    // Extract just the SBE message part for analysis
    std::vector<uint8_t> extracted_sbe(
        complete_packet.begin() + packet_header.size() + 2,
        complete_packet.end());
    printHex(extracted_sbe, "Extracted SBE Message");
}

int main() {
    analyzePacketStructure();
    return 0;
}