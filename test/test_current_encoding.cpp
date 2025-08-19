#include "messages/sbe_encoder.h"
#include "messages/mdp_messages.h"
#include <iostream>
#include <iomanip>

using namespace cme_mock;

void printHexDump(const std::vector<uint8_t>& data, const std::string& label) {
    std::cout << "\n" << label << " (" << data.size() << " bytes):" << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i % 16 == 0) {
            std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i] << " ";
        if ((i + 1) % 16 == 0 || i == data.size() - 1) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;
}

void analyzeHeader(const std::vector<uint8_t>& data, size_t offset, const std::string& label) {
    std::cout << "\n" << label << " at offset " << offset << ":" << std::endl;
    if (data.size() < offset + 8) {
        std::cout << "  Not enough data for SBE header" << std::endl;
        return;
    }
    
    uint16_t block_length = data[offset] | (data[offset+1] << 8);
    uint16_t template_id = data[offset+2] | (data[offset+3] << 8);
    uint16_t schema_id = data[offset+4] | (data[offset+5] << 8);
    uint16_t version = data[offset+6] | (data[offset+7] << 8);
    
    std::cout << "  Block Length: " << block_length << std::endl;
    std::cout << "  Template ID: " << template_id << std::endl;
    std::cout << "  Schema ID: " << schema_id << std::endl;
    std::cout << "  Version: " << version << std::endl;
}

int main() {
    std::cout << "Testing Current Mock Server Encoding" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Test incremental refresh
    std::cout << "\n=== INCREMENTAL REFRESH TEST ===" << std::endl;
    
    IncrementalRefresh inc;
    inc.header.sequence_number = 12345;
    inc.header.sending_time = 987654321;
    inc.header.msg_count = 1;
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
    
    // Test packet header
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        inc.header.sequence_number, inc.header.sending_time);
    printHexDump(packet_header, "Packet Header");
    
    // Test SBE message
    auto sbe_message = MDPMessageEncoder::encode_incremental_refresh(inc);
    printHexDump(sbe_message, "SBE Message");
    
    // Test combined packet (what feed_publisher creates)
    std::vector<uint8_t> complete_packet;
    complete_packet.reserve(packet_header.size() + sbe_message.size());
    complete_packet.insert(complete_packet.end(), packet_header.begin(), packet_header.end());
    complete_packet.insert(complete_packet.end(), sbe_message.begin(), sbe_message.end());
    
    printHexDump(complete_packet, "Complete Packet (Current Implementation)");
    
    // Analyze headers at different offsets
    analyzeHeader(complete_packet, 0, "SBE Header at offset 0");
    analyzeHeader(complete_packet, packet_header.size(), "SBE Header at offset " + std::to_string(packet_header.size()));
    
    std::cout << "\n=== PACKET STRUCTURE ===" << std::endl;
    std::cout << "Packet Header: " << packet_header.size() << " bytes (0-" << (packet_header.size()-1) << ")" << std::endl;
    std::cout << "SBE Message: " << sbe_message.size() << " bytes (" << packet_header.size() << "-" << (complete_packet.size()-1) << ")" << std::endl;
    std::cout << "Total: " << complete_packet.size() << " bytes" << std::endl;
    
    return 0;
}