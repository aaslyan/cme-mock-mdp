#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <cstdint>

// CME MDP 3.0 Binary Packet Header Structure
struct BinaryPacketHeader {
    uint32_t msg_seq_num;     // Offset 0: Message Sequence Number
    uint64_t sending_time;     // Offset 4: Sending Time (nanoseconds since Unix epoch)
    // Missing field at offset 12!
};

// CME MDP 3.0 expects this structure before each message:
struct MessageSize {
    uint16_t msg_size;         // Size of the following message (including SBE header)
};

// SBE Message Header (8 bytes)
struct SBEMessageHeader {
    uint16_t block_length;     // Offset 0: Block length of root message
    uint16_t template_id;      // Offset 2: Template ID
    uint16_t schema_id;        // Offset 4: Schema ID (should be 1 for CME)
    uint16_t version;          // Offset 6: Schema version (should be 13)
};

void print_hex_dump(const uint8_t* data, size_t size, const std::string& label) {
    std::cout << "\n" << label << " (" << size << " bytes):\n";
    std::cout << "Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII\n";
    std::cout << "------  -----------------------------------------------  ----------------\n";
    
    for (size_t i = 0; i < size; i += 16) {
        std::cout << std::setfill('0') << std::setw(6) << std::hex << i << "  ";
        
        // Hex bytes
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < size) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex 
                         << static_cast<int>(data[i + j]) << " ";
            } else {
                std::cout << "   ";
            }
        }
        
        std::cout << " ";
        
        // ASCII representation
        for (size_t j = 0; j < 16 && i + j < size; ++j) {
            char c = static_cast<char>(data[i + j]);
            if (c >= 32 && c <= 126) {
                std::cout << c;
            } else {
                std::cout << ".";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::dec;
}

void analyze_packet(const uint8_t* data, size_t size) {
    std::cout << "\n========================================\n";
    std::cout << "CME MDP 3.0 Packet Analysis\n";
    std::cout << "========================================\n";
    
    if (size < 12) {
        std::cout << "ERROR: Packet too small (< 12 bytes)\n";
        return;
    }
    
    size_t offset = 0;
    
    // Parse Binary Packet Header (but it's incomplete!)
    std::cout << "\nCURRENT Binary Packet Header (12 bytes):\n";
    uint32_t seq_num;
    uint64_t sending_time;
    std::memcpy(&seq_num, data + offset, 4);
    offset += 4;
    std::memcpy(&sending_time, data + offset, 8);
    offset += 8;
    
    std::cout << "  Sequence Number: " << seq_num << " (0x" << std::hex << seq_num << std::dec << ")\n";
    std::cout << "  Sending Time: " << sending_time << " (0x" << std::hex << sending_time << std::dec << ")\n";
    
    // PROBLEM: We're missing the message count field!
    std::cout << "\n⚠️  MISSING: Message Count field at offset 12!\n";
    std::cout << "  CME expects: uint16_t msg_count after sending_time\n";
    
    // Try to parse what we have as if it were correct
    if (offset + 2 <= size) {
        uint16_t msg_count;
        std::memcpy(&msg_count, data + offset, 2);
        std::cout << "  Current bytes at offset 12-13: 0x" << std::hex << msg_count << std::dec 
                  << " (interpreted as msg_count: " << msg_count << ")\n";
        offset += 2;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "EXPECTED CME MDP 3.0 Packet Structure:\n";
    std::cout << "========================================\n";
    std::cout << "Offset | Size | Field\n";
    std::cout << "-------|------|------------------------\n";
    std::cout << "  0    |  4   | Message Sequence Number\n";
    std::cout << "  4    |  8   | Sending Time (nanos)\n";
    std::cout << " 12    |  2   | Message Count (# of messages in packet)\n";
    std::cout << "-------|------|------------------------\n";
    std::cout << " 14    |  2   | Message Size (for message 1)\n";
    std::cout << " 16    |  8   | SBE Message Header\n";
    std::cout << " 24    |  var | SBE Message Body\n";
    std::cout << "  ...  |  2   | Message Size (for message 2, if any)\n";
    std::cout << "  ...  |  8   | SBE Message Header\n";
    std::cout << "  ...  |  var | SBE Message Body\n";
    
    // Now let's check if there's a message size field
    std::cout << "\n========================================\n";
    std::cout << "Attempting to parse first message:\n";
    std::cout << "========================================\n";
    
    if (offset + 2 <= size) {
        uint16_t msg_size;
        std::memcpy(&msg_size, data + offset, 2);
        std::cout << "Message Size field: " << msg_size << " bytes\n";
        offset += 2;
        
        if (offset + 8 <= size) {
            // Parse SBE header
            SBEMessageHeader sbe_header;
            std::memcpy(&sbe_header, data + offset, 8);
            
            std::cout << "\nSBE Message Header:\n";
            std::cout << "  Block Length: " << sbe_header.block_length << "\n";
            std::cout << "  Template ID: " << sbe_header.template_id << "\n";
            std::cout << "  Schema ID: " << sbe_header.schema_id << " (expected: 1)\n";
            std::cout << "  Version: " << sbe_header.version << " (expected: 13)\n";
            
            if (sbe_header.schema_id != 1) {
                std::cout << "  ⚠️  WARNING: Schema ID mismatch!\n";
            }
            if (sbe_header.version != 13) {
                std::cout << "  ⚠️  WARNING: Version mismatch!\n";
            }
        }
    }
    
    // Show the raw hex dump
    print_hex_dump(data, std::min(size, size_t(128)), "Raw Packet Data");
}

// Function to create a properly formatted CME packet
std::vector<uint8_t> create_correct_cme_packet() {
    std::vector<uint8_t> packet;
    
    // Binary Packet Header (14 bytes total)
    uint32_t seq_num = 1;
    uint64_t sending_time = 1234567890123456789ULL;
    uint16_t msg_count = 1;
    
    // Add sequence number (4 bytes)
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&seq_num), 
                  reinterpret_cast<uint8_t*>(&seq_num) + 4);
    
    // Add sending time (8 bytes)
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&sending_time),
                  reinterpret_cast<uint8_t*>(&sending_time) + 8);
    
    // Add message count (2 bytes) - THIS IS WHAT'S MISSING!
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&msg_count),
                  reinterpret_cast<uint8_t*>(&msg_count) + 2);
    
    // Now add the message size (2 bytes)
    uint16_t msg_size = 8 + 20; // SBE header + some body
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&msg_size),
                  reinterpret_cast<uint8_t*>(&msg_size) + 2);
    
    // Add SBE Message Header (8 bytes)
    SBEMessageHeader sbe_header;
    sbe_header.block_length = 20;
    sbe_header.template_id = 52;  // SnapshotFullRefresh52
    sbe_header.schema_id = 1;
    sbe_header.version = 13;
    
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&sbe_header),
                  reinterpret_cast<uint8_t*>(&sbe_header) + 8);
    
    // Add some dummy message body
    for (int i = 0; i < 20; ++i) {
        packet.push_back(0xAA);
    }
    
    return packet;
}

int main() {
    std::cout << "CME MDP 3.0 Binary Packet Analyzer\n";
    std::cout << "===================================\n\n";
    
    // Create a correct packet for reference
    auto correct_packet = create_correct_cme_packet();
    
    std::cout << "CORRECT CME MDP 3.0 Packet Structure:\n";
    analyze_packet(correct_packet.data(), correct_packet.size());
    
    std::cout << "\n\n========================================\n";
    std::cout << "DIAGNOSIS:\n";
    std::cout << "========================================\n";
    std::cout << "The issue is that the current implementation is missing the\n";
    std::cout << "2-byte 'Message Count' field at offset 12 in the Binary Packet Header.\n\n";
    std::cout << "Current structure (14 bytes total but wrong):\n";
    std::cout << "  [4 bytes] Sequence Number\n";
    std::cout << "  [8 bytes] Sending Time\n";
    std::cout << "  [2 bytes] Message Count (currently placed here)\n\n";
    std::cout << "But CME also expects a 2-byte Message Size before EACH message:\n";
    std::cout << "  [2 bytes] Message Size (size of following SBE message)\n";
    std::cout << "  [8 bytes] SBE Header\n";
    std::cout << "  [N bytes] SBE Body\n\n";
    std::cout << "So a complete single-message packet should be:\n";
    std::cout << "  14 bytes: Binary Packet Header (seq + time + count)\n";
    std::cout << "   2 bytes: Message Size\n";
    std::cout << "   8 bytes: SBE Message Header\n";
    std::cout << "   N bytes: SBE Message Body\n";
    
    return 0;
}