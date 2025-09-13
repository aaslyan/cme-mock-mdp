#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "utils/hexdump.h"
#include <iostream>
#include <vector>

using namespace cme_sbe;
using namespace cme_mock;

int main()
{
    std::cout << "=== Testing if SBE wrap automatically handles padding ===" << std::endl;
    
    // Test: Create the exact scenario that should produce 88 bytes
    std::vector<char> buffer(512, 0);
    
    // 1. Message header
    MessageHeader header;
    header.wrap(buffer.data(), 0, 0, buffer.size())
        .blockLength(MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(MDIncrementalRefreshBook46::sbeSchemaId())
        .version(MDIncrementalRefreshBook46::sbeSchemaVersion());
    
    size_t header_size = header.encodedLength();
    std::cout << "Header size: " << header_size << " bytes" << std::endl;
    
    // 2. Wrap message
    MDIncrementalRefreshBook46 sbe_msg;
    sbe_msg.wrapForEncode(buffer.data(), header_size, buffer.size());
    
    // 3. Set basic fields
    sbe_msg.transactTime(1234567890123456789ULL);
    
    // 4. Add 1 MD entry (to get close to 88 bytes)
    auto& md_group = sbe_msg.noMDEntriesCount(1);
    md_group.next()
        .securityID(100)
        .rptSeq(1)
        .mDPriceLevel(1)
        .numberOfOrders(1)
        .mDUpdateAction(MDUpdateAction::New)
        .mDEntryType(MDEntryTypeBook::Bid);
    md_group.mDEntryPx().mantissa(150000);
    md_group.mDEntrySize(10);
    
    // 5. Add 1 OrderID entry
    auto& order_group = sbe_msg.noOrderIDEntriesCount(1);
    order_group.next()
        .orderID(1000)
        .mDOrderPriority(1)
        .mDDisplayQty(5)
        .referenceID(1)
        .orderUpdateAction(OrderUpdateAction::New);
    
    // Check sizes before and after
    size_t message_length = sbe_msg.encodedLength();
    size_t total_length = header_size + message_length;
    
    std::cout << "Message encoded length: " << message_length << " bytes" << std::endl;
    std::cout << "Total length: " << total_length << " bytes" << std::endl;
    std::cout << "Expected for CME: 88 bytes" << std::endl;
    std::cout << "Difference: " << (88 - static_cast<int>(total_length)) << " bytes" << std::endl;
    
    // Check if buffer has any extra padding
    buffer.resize(total_length + 10); // Check a bit beyond
    std::vector<uint8_t> data(buffer.begin(), buffer.begin() + total_length + 8);
    
    std::cout << "\nBuffer content (including potential padding):" << std::endl;
    hexdump(data, "Message with potential padding");
    
    // Check what SBE puts at the end
    std::cout << "\nBytes immediately after message:" << std::endl;
    for (size_t i = total_length; i < total_length + 8; ++i) {
        std::cout << "  Offset " << i << ": 0x" << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(static_cast<uint8_t>(buffer[i])) << std::dec << std::endl;
    }
    
    // Test: Try creating message with larger buffer and see if SBE fills it
    std::cout << "\n=== Testing with pre-zeroed larger buffer ===" << std::endl;
    std::vector<char> large_buffer(200, 0x00); // Fill with zeros
    
    header.wrap(large_buffer.data(), 0, 0, large_buffer.size());
    sbe_msg.wrapForEncode(large_buffer.data(), header_size, large_buffer.size());
    
    // Set same data
    sbe_msg.transactTime(1234567890123456789ULL);
    auto& md_group2 = sbe_msg.noMDEntriesCount(1);
    md_group2.next().securityID(100).rptSeq(1).mDPriceLevel(1)
        .numberOfOrders(1).mDUpdateAction(MDUpdateAction::New).mDEntryType(MDEntryTypeBook::Bid);
    md_group2.mDEntryPx().mantissa(150000);
    md_group2.mDEntrySize(10);
    auto& order_group2 = sbe_msg.noOrderIDEntriesCount(1);
    order_group2.next().orderID(1000).mDOrderPriority(1).mDDisplayQty(5)
        .referenceID(1).orderUpdateAction(OrderUpdateAction::New);
    
    size_t new_length = sbe_msg.encodedLength();
    std::cout << "Same message in larger buffer: " << new_length << " bytes" << std::endl;
    
    if (new_length != message_length) {
        std::cout << "*** DIFFERENT LENGTH! SBE might be adding padding! ***" << std::endl;
    } else {
        std::cout << "Same length - SBE does not automatically add padding" << std::endl;
    }
    
    return 0;
}