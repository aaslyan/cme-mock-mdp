#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "utils/hexdump.h"
#include <iostream>
#include <vector>

using namespace cme_sbe;
using namespace cme_mock;

void test_message_combination(int md_entries, int order_entries, const std::string& description) {
    std::vector<char> buffer(512, 0);
    
    // 1. Message header
    MessageHeader header;
    header.wrap(buffer.data(), 0, 0, buffer.size())
        .blockLength(MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(MDIncrementalRefreshBook46::sbeSchemaId())
        .version(MDIncrementalRefreshBook46::sbeSchemaVersion());
    
    size_t header_size = header.encodedLength();
    
    // 2. Wrap message
    MDIncrementalRefreshBook46 sbe_msg;
    sbe_msg.wrapForEncode(buffer.data(), header_size, buffer.size());
    
    // 3. Set basic fields
    sbe_msg.transactTime(1234567890123456789ULL);
    
    // 4. Add MD entries
    auto& md_group = sbe_msg.noMDEntriesCount(static_cast<uint8_t>(md_entries));
    for (int i = 0; i < md_entries; ++i) {
        md_group.next()
            .securityID(100 + i)
            .rptSeq(i + 1)
            .mDPriceLevel(static_cast<uint8_t>(i + 1))
            .numberOfOrders(1 + i)
            .mDUpdateAction(MDUpdateAction::New)
            .mDEntryType(i % 2 == 0 ? MDEntryTypeBook::Bid : MDEntryTypeBook::Offer);
        
        md_group.mDEntryPx().mantissa(150000 + i * 100);
        md_group.mDEntrySize(10 + i * 5);
    }
    
    // 5. Add OrderID entries  
    auto& order_group = sbe_msg.noOrderIDEntriesCount(static_cast<uint8_t>(order_entries));
    for (int i = 0; i < order_entries; ++i) {
        order_group.next()
            .orderID(1000 + i)
            .mDOrderPriority(i + 1)
            .mDDisplayQty(5 + i * 2)
            .referenceID(static_cast<uint8_t>(i + 1))
            .orderUpdateAction(OrderUpdateAction::New);
    }
    
    // 6. Get sizes
    size_t message_length = sbe_msg.encodedLength();
    size_t total_length = header_size + message_length;
    
    std::cout << description << ":" << std::endl;
    std::cout << "  MD Entries: " << md_entries << ", Order Entries: " << order_entries << std::endl;
    std::cout << "  Message size: " << message_length << " bytes" << std::endl;
    std::cout << "  Total size: " << total_length << " bytes" << std::endl;
    
    // Check if it matches known CME sizes
    if (total_length == 88) {
        std::cout << "  *** MATCHES CME 88-byte message! ***" << std::endl;
    }
    if (total_length == 32) {
        std::cout << "  *** MATCHES CME 32-byte message! ***" << std::endl;
    }
    if (total_length >= 120 && total_length <= 132) {
        std::cout << "  *** CLOSE TO 132-byte range! ***" << std::endl;
    }
    
    std::cout << std::endl;
}

int main()
{
    std::cout << "=== Testing if our schema can produce 132-byte messages ===" << std::endl;
    std::cout << "CME observed: 132-byte packet with 88-byte + 32-byte messages" << std::endl;
    std::cout << "Target: Find combinations that produce ~120+ byte messages" << std::endl;
    std::cout << std::endl;
    
    // Test various combinations to find large messages
    test_message_combination(0, 0, "Empty message");
    test_message_combination(1, 0, "1 MD entry only");
    test_message_combination(1, 1, "1 MD + 1 Order (should be ~88)");
    test_message_combination(2, 1, "2 MD + 1 Order");
    test_message_combination(3, 1, "3 MD + 1 Order");
    test_message_combination(1, 2, "1 MD + 2 Order");
    test_message_combination(2, 2, "2 MD + 2 Order");
    test_message_combination(3, 2, "3 MD + 2 Order");
    test_message_combination(4, 2, "4 MD + 2 Order");
    test_message_combination(5, 2, "5 MD + 2 Order");
    test_message_combination(3, 3, "3 MD + 3 Order");
    test_message_combination(4, 3, "4 MD + 3 Order");
    test_message_combination(5, 3, "5 MD + 3 Order");
    
    // Test maximum entries (SBE usually supports up to 254)
    test_message_combination(10, 5, "10 MD + 5 Order");
    test_message_combination(15, 5, "15 MD + 5 Order");
    
    std::cout << "=== Analysis ===" << std::endl;
    std::cout << "Single message size breakdown:" << std::endl;
    std::cout << "  Base: 8 (header) + 11 (fixed) + 3 (MD header) + 3 (Order header) = 25 bytes" << std::endl;
    std::cout << "  Each MD Entry: 32 bytes" << std::endl;
    std::cout << "  Each Order Entry: 24 bytes" << std::endl;
    std::cout << std::endl;
    std::cout << "To get ~120+ bytes:" << std::endl;
    std::cout << "  Need: (120 - 25) = 95 bytes from entries" << std::endl;
    std::cout << "  Could be: 3 MD (96 bytes) or 2 MD + 1 Order (88 bytes) + padding" << std::endl;
    
    return 0;
}