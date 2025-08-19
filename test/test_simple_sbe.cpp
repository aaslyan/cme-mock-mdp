#include <iostream>
#include <vector>
#include <iomanip>
#include "cme_sbe/SnapshotFullRefresh52.h"
#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"

void printHex(const std::vector<char>& buffer, size_t length) {
    std::cout << "Hex: ";
    for (size_t i = 0; i < std::min(length, size_t(32)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << (unsigned char)buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

void analyzeHeader(const std::vector<char>& buffer) {
    if (buffer.size() < 8) {
        std::cout << "Buffer too small for header analysis" << std::endl;
        return;
    }
    
    // Parse header manually (little-endian)
    uint16_t blockLength = buffer[0] | (buffer[1] << 8);
    uint16_t templateId = buffer[2] | (buffer[3] << 8);
    uint16_t schemaId = buffer[4] | (buffer[5] << 8);
    uint16_t version = buffer[6] | (buffer[7] << 8);
    
    std::cout << "Parsed Header:" << std::endl;
    std::cout << "  Block Length: " << blockLength << std::endl;
    std::cout << "  Template ID: " << templateId << std::endl;
    std::cout << "  Schema ID: " << schemaId << std::endl;
    std::cout << "  Version: " << version << std::endl;
}

int main() {
    std::cout << "Testing Simple SBE Message Encoding" << std::endl;
    
    try {
        // Test incremental message with proper header encoding
        std::cout << "\n=== Incremental Message ===\n";
        
        std::vector<char> buffer(1024, 0);
        
        // Step 1: Encode the message header
        cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(), 
                                      cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
        header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
              .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
              .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
              .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
        
        std::cout << "Header encoded successfully" << std::endl;
        std::cout << "Template ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId() << std::endl;
        std::cout << "Schema ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId() << std::endl;
        std::cout << "Schema Version: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion() << std::endl;
        std::cout << "Block Length: " << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << std::endl;
        
        size_t header_size = header.encodedLength();
        std::cout << "Header size: " << header_size << " bytes" << std::endl;
        
        // Step 2: Encode the message body
        cme_sbe::MDIncrementalRefreshBook46 sbe_msg(
            buffer.data(), 
            header_size,
            buffer.size(),
            cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
            cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
        
        // Set basic field (just transactTime, no groups)
        sbe_msg.transactTime(123456789);
        
        std::cout << "Message body encoded successfully" << std::endl;
        
        size_t total_length = header_size + sbe_msg.encodedLength();
        std::cout << "Total message length: " << total_length << " bytes" << std::endl;
        
        buffer.resize(total_length);
        printHex(buffer, total_length);
        analyzeHeader(buffer);
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}