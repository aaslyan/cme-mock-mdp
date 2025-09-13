#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "utils/hexdump.h"
#include <iostream>
#include <vector>

using namespace cme_sbe;
using namespace cme_mock;

int main()
{
    std::cout << "=== Testing SBE Automatic Padding ===" << std::endl;
    
    // Test with different numbers of entries to see if SBE handles padding
    for (int md_entries = 0; md_entries <= 3; ++md_entries) {
        for (int order_entries = 0; order_entries <= 2; ++order_entries) {
            
            std::cout << "\n--- Test: " << md_entries << " MDEntries + " 
                      << order_entries << " OrderEntries ---" << std::endl;
            
            // Create large enough buffer
            std::vector<char> buffer(512, 0);
            
            // 1. Create and wrap message header
            MessageHeader header;
            header.wrap(buffer.data(), 0, 0, buffer.size())
                .blockLength(MDIncrementalRefreshBook46::sbeBlockLength())
                .templateId(MDIncrementalRefreshBook46::sbeTemplateId())
                .schemaId(MDIncrementalRefreshBook46::sbeSchemaId())
                .version(MDIncrementalRefreshBook46::sbeSchemaVersion());
            
            // 2. Wrap message for encoding
            size_t header_size = header.encodedLength();
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
                    .numberOfOrders(1)
                    .mDUpdateAction(MDUpdateAction::New)
                    .mDEntryType(MDEntryTypeBook::Bid);
                
                md_group.mDEntryPx().mantissa(150000 + i * 100);
                md_group.mDEntrySize(10 + i);
            }
            
            // 5. Add OrderID entries  
            auto& order_group = sbe_msg.noOrderIDEntriesCount(static_cast<uint8_t>(order_entries));
            for (int i = 0; i < order_entries; ++i) {
                order_group.next()
                    .orderID(1000 + i)
                    .mDOrderPriority(i + 1)
                    .mDDisplayQty(5 + i)
                    .referenceID(static_cast<uint8_t>(i + 1))
                    .orderUpdateAction(OrderUpdateAction::New);
            }
            
            // 6. Get encoded length
            size_t message_length = sbe_msg.encodedLength();
            size_t total_length = header_size + message_length;
            
            std::cout << "  Header size: " << header_size << " bytes" << std::endl;
            std::cout << "  Message size: " << message_length << " bytes" << std::endl;
            std::cout << "  Total size: " << total_length << " bytes" << std::endl;
            
            // Check 8-byte alignment
            bool is_8byte_aligned = (total_length % 8) == 0;
            std::cout << "  8-byte aligned: " << (is_8byte_aligned ? "YES" : "NO");
            if (!is_8byte_aligned) {
                size_t padding_needed = 8 - (total_length % 8);
                std::cout << " (needs " << padding_needed << " bytes padding)";
            }
            std::cout << std::endl;
            
            // Check if this matches CME sizes
            if (total_length == 88) {
                std::cout << "  *** MATCHES CME 88-byte message! ***" << std::endl;
            }
            if (total_length == 32) {
                std::cout << "  *** MATCHES CME 32-byte message! ***" << std::endl;
            }
            
            // Show hex dump of first part
            if (total_length <= 64) {
                buffer.resize(total_length);
                std::vector<uint8_t> data(buffer.begin(), buffer.end());
                hexdump(data, "Generated Message");
            }
        }
    }
    
    return 0;
}