#include <iostream>
#include <iomanip>
#include <vector>

// Let's analyze what the gateway client might be expecting
void analyzeCMEPacketFormat() {
    std::cout << "CME MDP 3.0 Packet Format Analysis" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Based on your error: bufferLength=46, offset=46
    // This suggests total packet is at least 92 bytes (46 + 46)
    // But our packet is only 68 bytes
    
    std::cout << "Current packet structure:" << std::endl;
    std::cout << "  0-11:  Packet Header (12 bytes)" << std::endl;
    std::cout << " 12-13:  Message Size (2 bytes)" << std::endl;
    std::cout << " 14-67:  SBE Message (54 bytes)" << std::endl;
    std::cout << " Total:  68 bytes" << std::endl;
    
    std::cout << "\nGateway client behavior:" << std::endl;
    std::cout << "  offset=46, bufferLength=46" << std::endl;
    std::cout << "  This means: total_packet_size >= 92 bytes" << std::endl;
    std::cout << "  And it's trying to read SBE at offset 46" << std::endl;
    
    std::cout << "\nPossible issues:" << std::endl;
    std::cout << "1. Gateway expects different packet framing" << std::endl;
    std::cout << "2. Gateway expects additional headers/fields" << std::endl;
    std::cout << "3. We're missing padding or alignment" << std::endl;
    std::cout << "4. Message size field interpretation differs" << std::endl;
    
    std::cout << "\nLet's check what should be at offset 46:" << std::endl;
    
    // Our current packet structure
    std::vector<uint8_t> our_packet = {
        // Packet header (12 bytes)
        0x39, 0x30, 0x00, 0x00, 0xb1, 0x68, 0xde, 0x3a, 0x00, 0x00, 0x00, 0x00,
        // Message size (2 bytes) 
        0x36, 0x00,
        // SBE message (54 bytes)
        0x0b, 0x00, 0x2e, 0x00, 0x01, 0x00, 0x0d, 0x00, 0x15, 0xcd, 0x5b, 0x07, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x01, 0x10, 0x27,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    std::cout << "Our packet at offset 46: ";
    if (our_packet.size() > 46) {
        for (size_t i = 46; i < std::min(our_packet.size(), size_t(54)); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)our_packet[i] << " ";
        }
        std::cout << std::endl;
        
        // Bytes 46-53 in our packet are: 01 00 00 00 00 00 00 00
        // This is NOT a valid SBE message header!
    } else {
        std::cout << "BEYOND PACKET END!" << std::endl;
    }
    
    std::cout << "\nCorrect SBE message starts at offset 14: ";
    for (size_t i = 14; i < std::min(our_packet.size(), size_t(22)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)our_packet[i] << " ";
    }
    std::cout << " (SBE MessageHeader)" << std::endl;
    
    std::cout << "\nConclusion: Gateway client has wrong offset calculation!" << std::endl;
    std::cout << "It should read SBE at offset 14, not 46." << std::endl;
}

void proposeSolution() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PROPOSED SOLUTIONS" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    std::cout << "\n1. Check gateway client packet parsing logic:" << std::endl;
    std::cout << "   - Verify it correctly skips packet header (12 bytes)" << std::endl;
    std::cout << "   - Verify it correctly reads message size (2 bytes)" << std::endl;
    std::cout << "   - Verify it starts SBE decoding at correct offset" << std::endl;
    
    std::cout << "\n2. Alternative packet format (if gateway expects different):" << std::endl;
    std::cout << "   - Maybe gateway expects message count field?" << std::endl;
    std::cout << "   - Maybe gateway expects additional MDP headers?" << std::endl;
    std::cout << "   - Maybe padding/alignment is required?" << std::endl;
    
    std::cout << "\n3. Test with minimal packet:" << std::endl;
    std::cout << "   - Send just SBE message without packet header" << std::endl;
    std::cout << "   - Send with only message size + SBE message" << std::endl;
    
    std::cout << "\n4. Debug gateway client expectations:" << std::endl;
    std::cout << "   - Log exactly what bytes it's reading" << std::endl;
    std::cout << "   - Check its packet parsing state machine" << std::endl;
}

int main() {
    analyzeCMEPacketFormat();
    proposeSolution();
    return 0;
}