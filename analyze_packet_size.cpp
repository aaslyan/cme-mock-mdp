#include "messages/cme_sbe_encoder.h"
#include "utils/hexdump.h"
#include <iostream>

using namespace cme_mock;

int main()
{
    std::cout << "=== Analyzing CME Packet Sizes ===" << std::endl;
    
    // Create incremental refresh similar to actual CME data
    IncrementalRefresh refresh;
    refresh.transact_time = 1234567890123456789ULL;
    
    // Add a single price level entry
    MDPriceLevel level;
    level.security_id = 100;
    level.rpt_seq = 1;
    level.price = 150000; // $1500.00 scaled
    level.quantity = 10;
    level.number_of_orders = 1;
    level.price_level = 1;
    level.entry_type = MDEntryType::Bid;
    level.update_action = MDUpdateAction::New;
    refresh.price_levels.push_back(level);
    
    // Generate our SBE message
    auto sbe_message = CMESBEEncoder::encode_incremental_refresh(refresh);
    std::cout << "Our SBE message size: " << sbe_message.size() << " bytes" << std::endl;
    
    // Generate complete packet
    std::vector<std::vector<uint8_t>> messages = { sbe_message };
    auto packet = CMESBEEncoder::encode_multi_message_packet(12345, 987654321ULL, messages);
    std::cout << "Our complete packet size: " << packet.size() << " bytes" << std::endl;
    
    std::cout << "\nActual CME packet breakdown:" << std::endl;
    std::cout << "  Total packet: 132 bytes" << std::endl;
    std::cout << "  Message size: 88 bytes" << std::endl;
    std::cout << "  Packet header: 12 bytes" << std::endl;
    std::cout << "  Message size field: 2 bytes" << std::endl;
    std::cout << "  Expected: 12 + 2 + 88 = 102 bytes" << std::endl;
    std::cout << "  Remaining: 132 - 102 = 30 bytes (possibly padding or another message)" << std::endl;
    
    std::cout << "\nOur packet breakdown:" << std::endl;
    std::cout << "  Total packet: " << packet.size() << " bytes" << std::endl;
    std::cout << "  SBE message: " << sbe_message.size() << " bytes" << std::endl;
    std::cout << "  Packet header: 12 bytes" << std::endl;
    std::cout << "  Message size field: 2 bytes" << std::endl;
    std::cout << "  Expected: 12 + 2 + " << sbe_message.size() << " = " << (12 + 2 + sbe_message.size()) << " bytes" << std::endl;
    
    std::cout << "\nSize comparison:" << std::endl;
    std::cout << "  CME SBE message: 88 bytes" << std::endl;
    std::cout << "  Our SBE message: " << sbe_message.size() << " bytes" << std::endl;
    std::cout << "  Difference: " << (88 - static_cast<int>(sbe_message.size())) << " bytes" << std::endl;
    
    if (sbe_message.size() != 88) {
        std::cout << "\n⚠️  SIZE MISMATCH DETECTED!" << std::endl;
        std::cout << "This could be causing parsing crashes." << std::endl;
        
        if (sbe_message.size() < 88) {
            std::cout << "Our message is " << (88 - sbe_message.size()) << " bytes too small." << std::endl;
            std::cout << "Missing fields or incorrect padding?" << std::endl;
        } else {
            std::cout << "Our message is " << (sbe_message.size() - 88) << " bytes too large." << std::endl;
            std::cout << "Extra fields or incorrect encoding?" << std::endl;
        }
    }
    
    // Show hex dump of our message
    std::cout << "\nOur SBE message hex dump:" << std::endl;
    hexdump(sbe_message, "Our SBE Message");
    
    return 0;
}