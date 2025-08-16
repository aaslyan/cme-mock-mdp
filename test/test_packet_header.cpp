#include <iostream>
#include <iomanip>
#include <vector>
#include "messages/sbe_encoder.h"

using namespace cme_mock;

int main() {
    // Test packet header encoding
    uint32_t seq = 0x01af;
    uint64_t timestamp = 0x185c42f20bf95900;
    
    auto header = MDPMessageEncoder::encode_packet_header(seq, timestamp);
    
    std::cout << "Packet header size: " << header.size() << " bytes" << std::endl;
    std::cout << "Expected: 12 bytes (4 seq + 8 timestamp)" << std::endl;
    std::cout << "Hex: ";
    for (auto b : header) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)b << " ";
    }
    std::cout << std::dec << std::endl;
    
    // Decode to verify
    if (header.size() >= 12) {
        uint32_t decoded_seq = header[0] | (header[1] << 8) | (header[2] << 16) | (header[3] << 24);
        std::cout << "\nDecoded sequence: " << decoded_seq << " (expected " << seq << ")" << std::endl;
    }
    
    return 0;
}