#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/SnapshotFullRefresh52.h"
#include <iomanip>
#include <iostream>
#include <vector>

void printHex(const std::vector<char>& buffer, size_t length)
{
    std::cout << "Hex: ";
    for (size_t i = 0; i < std::min(length, size_t(32)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (unsigned char)buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

int main()
{
    std::cout << "Testing SBE Message Encoding" << std::endl;

    // Test buffer
    std::vector<char> buffer(1024, 0);

    try {
        // Test incremental message
        std::cout << "\n=== Incremental Message ===" << std::endl;
        cme_sbe::MDIncrementalRefreshBook46 inc_msg(buffer.data(), buffer.size());

        std::cout << "Before setting fields:" << std::endl;
        std::cout << "Template ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId() << std::endl;
        std::cout << "Schema ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId() << std::endl;
        std::cout << "Schema Version: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion() << std::endl;
        std::cout << "Block Length: " << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << std::endl;
        std::cout << "Encoded Length: " << inc_msg.encodedLength() << std::endl;

        // Set a basic field
        inc_msg.transactTime(123456789);

        std::cout << "After setting transactTime:" << std::endl;
        std::cout << "Encoded Length: " << inc_msg.encodedLength() << std::endl;
        printHex(buffer, inc_msg.encodedLength());

        // Look at just the header part
        std::cout << "First 16 bytes (likely header):" << std::endl;
        printHex(buffer, 16);

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}