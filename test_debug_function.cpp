#include "cme_debug_function.h"
#include <iostream>
#include <vector>

int main()
{
    std::cout << "Testing CME Debug Function" << std::endl;
    std::cout << "==========================" << std::endl;

    // Example actual CME data (132 bytes with 88-byte and 32-byte messages)
    std::vector<uint8_t> real_cme_data = {
        // Packet header (12 bytes)
        0x01, 0x00, 0x00, 0x00, // sequence = 1
        0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, // timestamp

        // Message 1: 88 bytes
        0x58, 0x00, // message size = 88
        // SBE Header (8 bytes)
        0x0B, 0x00, // block_length = 11
        0x2E, 0x00, // template_id = 46
        0x01, 0x00, // schema_id = 1
        0x0D, 0x00, // version = 13
        // Message body (80 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // transactTime
        0x01, 0x00, 0x00, 0x00, // securityID = 1
        0x01, 0x00, 0x00, 0x00, // rptSeq = 1
        0x02, 0x00, // noMDEntries = 2
        // MD Entry 1
        0x01, 0x00, 0x00, 0x00, // securityID = 1
        0x01, 0x00, 0x00, 0x00, // rptSeq = 1
        0x01, // mDPriceLevel = 1
        0x30, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // price (30000)
        0x64, 0x00, 0x00, 0x00, // size = 100
        0x01, 0x00, // numberOfOrders = 1
        // MD Entry 2
        0x01, 0x00, 0x00, 0x00, // securityID = 1
        0x02, 0x00, 0x00, 0x00, // rptSeq = 2
        0x02, // mDPriceLevel = 2
        0x94, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // price (30100)
        0xC8, 0x00, 0x00, 0x00, // size = 200
        0x02, 0x00, // numberOfOrders = 2
        // OrderID entries (this is what's often missing in mock servers!)
        0x02, 0x00, // noOrderIDEntries = 2
        // Order 1
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // orderID = 1
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // priority = 1
        0x64, 0x00, 0x00, 0x00, // displayQty = 100
        0x01, // referenceID = 1
        // Order 2
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // orderID = 2
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // priority = 2
        0xC8, 0x00, 0x00, 0x00, // displayQty = 200
        0x02, // referenceID = 2

        // Message 2: 32 bytes (second message in packet)
        0x20, 0x00, // message size = 32
        // SBE Header
        0x0B, 0x00, // block_length = 11
        0x2E, 0x00, // template_id = 46
        0x01, 0x00, // schema_id = 1
        0x0D, 0x00, // version = 13
        // Smaller message body (24 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // transactTime
        0x02, 0x00, 0x00, 0x00, // securityID = 2
        0x01, 0x00, 0x00, 0x00, // rptSeq = 1
        0x01, 0x00, // noMDEntries = 1
        0x01, 0x00, // noOrderIDEntries = 1
        0x00, 0x00 // padding to reach 32 bytes
    };

    std::cout << "Analyzing " << real_cme_data.size() << " bytes of simulated CME data...\n"
              << std::endl;

    // Call the debug function
    std::string analysis = analyze_cme_data_for_mock_server_debug(real_cme_data.data(), real_cme_data.size());

    std::cout << analysis << std::endl;

    std::cout << "\n"
              << std::string(50, '=') << std::endl;
    std::cout << "Function ready for integration into CME parser!" << std::endl;
    std::cout << "Usage: analyze_cme_data_for_mock_server_debug(data_ptr, length)" << std::endl;

    return 0;
}