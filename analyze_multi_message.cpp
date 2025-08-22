#include <iostream>
#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"

int main() {
    std::cout << "=== CME Multi-Message Packet Analysis ===" << std::endl;
    
    // Your observations
    int msg_size = 11;
    int block_length = 11;
    
    std::cout << "Your observations:" << std::endl;
    std::cout << "  Message Size: " << msg_size << " bytes" << std::endl;
    std::cout << "  Block Length: " << block_length << " bytes" << std::endl;
    
    // SBE structure analysis
    int sbe_header = 8; // MessageHeader
    int sbe_block = cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength();
    
    std::cout << "\nSBE Analysis:" << std::endl;
    std::cout << "  SBE MessageHeader: " << sbe_header << " bytes" << std::endl;
    std::cout << "  SBE Block Length: " << sbe_block << " bytes" << std::endl;
    
    std::cout << "\nKey insight: msgsize=" << msg_size << " suggests:" << std::endl;
    std::cout << "  - This is ONLY the message body size (no header)" << std::endl;
    std::cout << "  - blockLength=" << block_length << " matches perfectly!" << std::endl;
    std::cout << "  - Total SBE message = " << sbe_header << " + " << msg_size << " = " << (sbe_header + msg_size) << " bytes" << std::endl;
    
    std::cout << "\nMulti-message packet structure:" << std::endl;
    std::cout << "  Binary Packet Header: 12 bytes (seq + timestamp)" << std::endl;
    std::cout << "  Message Count: 2 bytes (how many messages follow)" << std::endl;
    std::cout << "  For each message:" << std::endl;
    std::cout << "    Message Size: 2 bytes (" << msg_size << " = body only)" << std::endl;
    std::cout << "    SBE MessageHeader: " << sbe_header << " bytes" << std::endl;
    std::cout << "    SBE Message Body: " << msg_size << " bytes" << std::endl;
    
    std::cout << "\nSo a single message packet would be:" << std::endl;
    int single_packet = 12 + 2 + 2 + 8 + 11;
    std::cout << "  12 + 2 + 2 + 8 + 11 = " << single_packet << " bytes" << std::endl;
    
    std::cout << "\nMultiple messages in one packet:" << std::endl;
    std::cout << "  Packet Header (14 bytes) + Message1 (21 bytes) + Message2 (21 bytes) + ..." << std::endl;
    
    return 0;
}