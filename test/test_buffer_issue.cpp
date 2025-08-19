#include <iostream>
#include <vector>
#include <iomanip>
#include "cme_sbe/SnapshotFullRefresh52.h"
#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"

void printHex(const std::vector<char>& buffer, size_t length) {
    std::cout << "Hex: ";
    for (size_t i = 0; i < std::min(length, size_t(64)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << (unsigned char)buffer[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n     ";
    }
    std::cout << std::dec << std::endl;
}

void testBufferSizes() {
    std::cout << "Testing different buffer sizes and patterns..." << std::endl;
    
    // Test different buffer sizes
    for (size_t buf_size = 64; buf_size <= 2048; buf_size *= 2) {
        std::cout << "\n=== Testing buffer size: " << buf_size << " ===\n";
        
        try {
            std::vector<char> buffer(buf_size, 0);
            
            // Test 1: MessageHeader alone
            std::cout << "Test 1: MessageHeader construction\n";
            cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(), 
                                          cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
            
            header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
                  .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
                  .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
                  .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
            
            std::cout << "  Header encoded successfully, size: " << header.encodedLength() << "\n";
            
            // Test 2: Message body construction
            std::cout << "Test 2: Message body construction\n";
            size_t header_size = header.encodedLength();
            size_t remaining_size = buffer.size() - header_size;
            std::cout << "  Header size: " << header_size << ", Remaining: " << remaining_size << "\n";
            
            if (remaining_size < cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength()) {
                std::cout << "  ERROR: Not enough space for message body\n";
                continue;
            }
            
            cme_sbe::MDIncrementalRefreshBook46 sbe_msg(
                buffer.data(), 
                header_size,
                buffer.size(),
                cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
                cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
            
            std::cout << "  Message body created successfully\n";
            
            // Test 3: Set basic field
            std::cout << "Test 3: Setting transactTime\n";
            sbe_msg.transactTime(123456789);
            std::cout << "  transactTime set successfully\n";
            
            // Test 4: Group with count 0
            std::cout << "Test 4: Creating empty group\n";
            auto& entries_group = sbe_msg.noMDEntriesCount(0);
            std::cout << "  Empty group created successfully\n";
            
            // Test 5: Group with count 1
            std::cout << "Test 5: Creating group with 1 entry\n";
            auto& entries_group1 = sbe_msg.noMDEntriesCount(1);
            std::cout << "  Group with 1 entry created successfully\n";
            
            entries_group1.next()
                .securityID(31)
                .rptSeq(1)
                .mDPriceLevel(1);
            std::cout << "  Entry fields set successfully\n";
            
            size_t total_length = header_size + sbe_msg.encodedLength();
            std::cout << "  Total encoded length: " << total_length << "\n";
            
            if (total_length <= buffer.size()) {
                std::cout << "SUCCESS: Buffer size " << buf_size << " works!\n";
                buffer.resize(total_length);
                printHex(buffer, total_length);
                break; // Found working size
            } else {
                std::cout << "ERROR: Encoded length exceeds buffer\n";
            }
            
        } catch (const std::exception& e) {
            std::cout << "ERROR with buffer size " << buf_size << ": " << e.what() << "\n";
        }
    }
}

int main() {
    std::cout << "SBE Buffer Size Investigation" << std::endl;
    std::cout << "============================" << std::endl;
    
    std::cout << "\nSBE Constants:" << std::endl;
    std::cout << "MessageHeader::encodedLength(): " << cme_sbe::MessageHeader::encodedLength() << std::endl;
    std::cout << "MDIncrementalRefreshBook46::sbeBlockLength(): " << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << std::endl;
    std::cout << "MDIncrementalRefreshBook46::sbeTemplateId(): " << cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId() << std::endl;
    std::cout << "SnapshotFullRefresh52::sbeBlockLength(): " << cme_sbe::SnapshotFullRefresh52::sbeBlockLength() << std::endl;
    
    testBufferSizes();
    
    return 0;
}