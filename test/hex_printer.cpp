#include "network/udp_receiver.h"
#include "utils/logger.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace cme_mock;

void printDetailedHex(const std::vector<uint8_t>& data) {
    std::cout << "\n=== HEX ANALYSIS (" << data.size() << " bytes) ===" << std::endl;
    
    // Print raw hex dump
    std::cout << "\nRAW HEX DUMP:" << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i % 16 == 0) {
            std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i] << " ";
        if ((i + 1) % 16 == 0 || i == data.size() - 1) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;
    
    // Print one-line hex for easy copying
    std::cout << "ONE-LINE HEX: ";
    for (size_t i = 0; i < data.size() && i < 64; ++i) {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i] << " ";
    }
    if (data.size() > 64) std::cout << "... (truncated)";
    std::cout << std::dec << std::endl;
    
    // Analyze potential SBE headers at different offsets
    std::cout << "\nPOTENTIAL SBE HEADERS:" << std::endl;
    for (size_t offset = 0; offset <= std::min(data.size(), size_t(50)); offset += 2) {
        if (data.size() < offset + 8) break;
        
        uint16_t block_length = data[offset] | (data[offset+1] << 8);
        uint16_t template_id = data[offset+2] | (data[offset+3] << 8);
        uint16_t schema_id = data[offset+4] | (data[offset+5] << 8);
        uint16_t version = data[offset+6] | (data[offset+7] << 8);
        
        // Only print if it looks like a reasonable SBE header
        if (template_id > 0 && template_id < 1000 && schema_id > 0 && schema_id < 100 && version > 0 && version < 100) {
            std::cout << "Offset " << std::setw(2) << offset << ": ";
            std::cout << "BlockLen=" << std::setw(3) << block_length;
            std::cout << " TemplateID=" << std::setw(2) << template_id;
            std::cout << " SchemaID=" << schema_id;
            std::cout << " Version=" << std::setw(2) << version;
            
            // Highlight likely candidates
            if (template_id == 46 || template_id == 52) {
                std::cout << " <-- POSSIBLE CME TEMPLATE!";
            }
            if (schema_id == 1 && version == 13) {
                std::cout << " <-- CORRECT SCHEMA V13!";
            }
            std::cout << std::endl;
        }
    }
    
    // Print packet structure analysis
    std::cout << "\nPACKET STRUCTURE ANALYSIS:" << std::endl;
    
    if (data.size() >= 12) {
        std::cout << "Bytes 0-11 (Packet Header):" << std::endl;
        uint32_t sequence = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        uint64_t timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            timestamp |= static_cast<uint64_t>(data[4 + i]) << (i * 8);
        }
        std::cout << "  Sequence: " << sequence << std::endl;
        std::cout << "  Timestamp: " << timestamp << std::endl;
    }
    
    if (data.size() >= 20) {
        std::cout << "Bytes 12-19 (Potential SBE Header):" << std::endl;
        uint16_t block_length = data[12] | (data[13] << 8);
        uint16_t template_id = data[14] | (data[15] << 8);
        uint16_t schema_id = data[16] | (data[17] << 8);
        uint16_t version = data[18] | (data[19] << 8);
        
        std::cout << "  Block Length: " << block_length << std::endl;
        std::cout << "  Template ID: " << template_id << std::endl;
        std::cout << "  Schema ID: " << schema_id << std::endl;
        std::cout << "  Version: " << version << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
}

int main() {
    std::cout << "CME Gateway Client Hex Analyzer" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "This tool captures UDP packets and prints detailed hex analysis" << std::endl;
    std::cout << "Copy the hex output and send it for analysis" << std::endl;
    
    Logger::instance().set_level(LogLevel::ERROR); // Minimize logging
    
    // Listen to incremental feed on multicast address
    UDPReceiver receiver("224.0.28.64", 14310);
    if (!receiver.initialize()) {
        std::cerr << "Failed to initialize UDP receiver on multicast 224.0.28.64:14310" << std::endl;
        std::cerr << "Trying localhost fallback..." << std::endl;
        
        // Fallback to localhost
        UDPReceiver receiver_local("127.0.0.1", 14310);
        if (!receiver_local.initialize()) {
            std::cerr << "Failed to initialize UDP receiver on localhost 127.0.0.1:14310" << std::endl;
            return 1;
        }
        
        std::cout << "\nListening on localhost 127.0.0.1:14310..." << std::endl;
        return 0; // Need to handle this properly, but for now just exit
    }
    
    std::cout << "\nListening on multicast 224.0.28.64:14310..." << std::endl;
    std::cout << "Waiting for packets... (Ctrl+C to stop)" << std::endl;
    
    int packet_count = 0;
    while (packet_count < 10) { // Capture first 10 packets
        std::vector<uint8_t> data;
        if (receiver.receive_once(data, 2000)) { // 2 second timeout
            packet_count++;
            std::cout << "\n" << std::string(60, '=') << std::endl;
            std::cout << "PACKET " << packet_count << " RECEIVED" << std::endl;
            printDetailedHex(data);
        } else {
            std::cout << "." << std::flush;
        }
    }
    
    std::cout << "\nCapture complete. Copy the hex output above." << std::endl;
    return 0;
}