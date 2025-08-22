#include "utils/hexdump.h"
#include <iostream>
#include <vector>

using namespace cme_mock;

int main()
{
    // Test with sample packet data
    std::vector<uint8_t> sample_packet = {
        0x39, 0x30, 0x00, 0x00, // Sequence number: 12345
        0x15, 0xCD, 0x5B, 0x07, 0x00, 0x00, 0x00, 0x00, // Timestamp
        0x50, 0x00, // Message size: 80 bytes
        0x0B, 0x00, 0x2E, 0x00, 0x01, 0x00, 0x0D, 0x00, // SBE header
        0x15, 0xCD, 0x5B, 0x07, 0x00, 0x00, 0x00, 0x00, // transactTime
        0x08, 0x00, 0x00 // matchEventIndicator + padding
    };

    std::cout << "=== Testing hexdump functions ===" << std::endl;

    // Test standard hexdump
    hexdump(sample_packet, "Sample CME Packet");

    std::cout << std::endl;

    // Test compact hexdump
    hexdump_compact(sample_packet, "Compact");

    std::cout << std::endl;

    // Test with raw pointer
    uint8_t raw_data[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE };
    hexdump(raw_data, sizeof(raw_data), "Raw pointer test");

    std::cout << std::endl;

    // Test hexdump_string
    std::string hex_str = hexdump_string(raw_data, sizeof(raw_data));
    std::cout << "String output:" << std::endl
              << hex_str << std::endl;

    return 0;
}