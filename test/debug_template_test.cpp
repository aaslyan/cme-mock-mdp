#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>

void analyze_packet(const uint8_t* data, size_t size) {
    std::cout << "\n=== CME MDP 3.0 Packet Analysis ===\n";
    std::cout << "Total packet size: " << size << " bytes\n\n";
    
    if (size < 14) {
        std::cout << "ERROR: Packet too small for binary header\n";
        return;
    }
    
    // Parse Binary Packet Header (14 bytes)
    uint32_t seq_num;
    uint64_t sending_time;
    uint16_t msg_count;
    
    std::memcpy(&seq_num, data, 4);
    std::memcpy(&sending_time, data + 4, 8);
    std::memcpy(&msg_count, data + 12, 2);
    
    std::cout << "Binary Packet Header:\n";
    std::cout << "  Sequence Number: " << seq_num << "\n";
    std::cout << "  Sending Time: " << sending_time << "\n";
    std::cout << "  Message Count: " << msg_count << "\n\n";
    
    size_t offset = 14;
    
    // Parse each message
    for (uint16_t msg_idx = 0; msg_idx < msg_count && offset < size; ++msg_idx) {
        std::cout << "Message " << (msg_idx + 1) << ":\n";
        
        if (offset + 2 > size) {
            std::cout << "  ERROR: Not enough data for message size\n";
            break;
        }
        
        // Read message size
        uint16_t msg_size;
        std::memcpy(&msg_size, data + offset, 2);
        offset += 2;
        
        std::cout << "  Message Size: " << msg_size << " bytes\n";
        
        if (offset + msg_size > size) {
            std::cout << "  ERROR: Message size exceeds packet\n";
            break;
        }
        
        // Parse SBE header (8 bytes)
        if (msg_size >= 8) {
            uint16_t block_length, template_id, schema_id, version;
            std::memcpy(&block_length, data + offset, 2);
            std::memcpy(&template_id, data + offset + 2, 2);
            std::memcpy(&schema_id, data + offset + 4, 2);
            std::memcpy(&version, data + offset + 6, 2);
            
            std::cout << "  SBE Header:\n";
            std::cout << "    Block Length: " << block_length << "\n";
            std::cout << "    Template ID: " << template_id;
            
            // Identify known template IDs
            switch(template_id) {
                case 4: std::cout << " (ChannelReset)"; break;
                case 12: std::cout << " (AdminHeartbeat)"; break;
                case 30: std::cout << " (SecurityStatus)"; break;
                case 32: std::cout << " *** UNKNOWN TEMPLATE 32! ***"; break;
                case 37: std::cout << " (MDIncrementalRefreshVolume)"; break;
                case 39: std::cout << " (QuoteRequest)"; break;
                case 46: std::cout << " (MDIncrementalRefreshBook)"; break;
                case 48: std::cout << " (MDIncrementalRefreshTrade)"; break;
                case 49: std::cout << " (MDIncrementalRefreshDailyStatistics)"; break;
                case 50: std::cout << " (MDIncrementalRefreshLimitsBanding)"; break;
                case 51: std::cout << " (MDIncrementalRefreshSessionStatistics)"; break;
                case 52: std::cout << " (SnapshotFullRefresh)"; break;
                case 53: std::cout << " (SnapshotFullRefreshOrderBook)"; break;
                case 54: std::cout << " (MDInstrumentDefinitionFuture)"; break;
                case 55: std::cout << " (MDInstrumentDefinitionOption)"; break;
                case 56: std::cout << " (MDInstrumentDefinitionSpread)"; break;
                case 57: std::cout << " (MDInstrumentDefinitionFixedIncome)"; break;
                case 58: std::cout << " (MDInstrumentDefinitionRepo)"; break;
                case 59: std::cout << " (SnapshotRefreshTopOrders)"; break;
                case 60: std::cout << " (SecurityStatusWorkup)"; break;
                case 61: std::cout << " (SnapshotFullRefreshTCP)"; break;
                case 62: std::cout << " (CollateralMarketValue)"; break;
                case 63: std::cout << " (MDInstrumentDefinitionFX)"; break;
                default: std::cout << " (Unknown)"; break;
            }
            std::cout << "\n";
            
            std::cout << "    Schema ID: " << schema_id << " (expected: 1)\n";
            std::cout << "    Version: " << version << " (expected: 13 for FX, 9 for equity)\n";
            
            if (template_id == 32) {
                std::cout << "\n  *** PROBLEM: Template ID 32 is not a valid CME MDP 3.0 template! ***\n";
                std::cout << "  This might be caused by:\n";
                std::cout << "  1. Incorrect byte order (endianness issue)\n";
                std::cout << "  2. Wrong offset - reading data from wrong position\n";
                std::cout << "  3. Block length value being misinterpreted as template ID\n";
                
                // Show hex dump of the problematic area
                std::cout << "\n  Hex dump around SBE header:\n  ";
                for (size_t i = offset; i < offset + 16 && i < size; ++i) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') 
                             << static_cast<int>(data[i]) << " ";
                }
                std::cout << std::dec << "\n";
            }
        }
        
        offset += msg_size;
    }
    
    // Show complete hex dump
    std::cout << "\nComplete packet hex dump:\n";
    std::cout << "Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n";
    std::cout << "------  -----------------------------------------------\n";
    
    for (size_t i = 0; i < size && i < 256; i += 16) {
        std::cout << std::setfill('0') << std::setw(6) << std::hex << i << "  ";
        
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < size) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex 
                         << static_cast<int>(data[i + j]) << " ";
            } else {
                std::cout << "   ";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::dec;
}

// Test with a sample packet
int main() {
    std::cout << "CME MDP 3.0 Template ID Debugger\n";
    std::cout << "=================================\n\n";
    
    // Create a test packet to see what we're generating
    std::vector<uint8_t> packet;
    
    // Binary Packet Header
    uint32_t seq = 1;
    uint64_t time = 1234567890123456789ULL;
    uint16_t count = 1;
    
    packet.insert(packet.end(), (uint8_t*)&seq, (uint8_t*)&seq + 4);
    packet.insert(packet.end(), (uint8_t*)&time, (uint8_t*)&time + 8);
    packet.insert(packet.end(), (uint8_t*)&count, (uint8_t*)&count + 2);
    
    // Message size
    uint16_t msg_size = 32; // Typical size
    packet.insert(packet.end(), (uint8_t*)&msg_size, (uint8_t*)&msg_size + 2);
    
    // SBE Header
    uint16_t block_length = 32;  // If this gets read as template ID, we'd see 32!
    uint16_t template_id = 52;   // SnapshotFullRefresh
    uint16_t schema_id = 1;
    uint16_t version = 13;
    
    packet.insert(packet.end(), (uint8_t*)&block_length, (uint8_t*)&block_length + 2);
    packet.insert(packet.end(), (uint8_t*)&template_id, (uint8_t*)&template_id + 2);
    packet.insert(packet.end(), (uint8_t*)&schema_id, (uint8_t*)&schema_id + 2);
    packet.insert(packet.end(), (uint8_t*)&version, (uint8_t*)&version + 2);
    
    // Add some dummy body
    for (int i = 0; i < 24; ++i) {
        packet.push_back(0);
    }
    
    std::cout << "Test Case 1: Correct packet structure\n";
    analyze_packet(packet.data(), packet.size());
    
    // Now test what happens if block_length is 32
    std::cout << "\n\n=================================\n";
    std::cout << "Test Case 2: Block length = 32 (might be the issue)\n";
    
    // Modify the block length to 32 to see if this causes the problem
    block_length = 32;
    std::memcpy(packet.data() + 16, &block_length, 2);
    
    analyze_packet(packet.data(), packet.size());
    
    return 0;
}