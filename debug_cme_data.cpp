#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace cme_sbe;

// Include the debug function from test_client.cpp
std::string analyze_for_mock_server_debug(const uint8_t* data, size_t length)
{
    std::string analysis = "=== CME DATA ANALYSIS FOR MOCK SERVER DEBUG ===\n";
    analysis += "Packet size: " + std::to_string(length) + " bytes\n\n";
    
    if (length < 12) {
        return analysis + "ERROR: Packet too small for CME header\n";
    }
    
    try {
        size_t offset = 0;
        
        // Parse packet header
        uint32_t sequence = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        uint64_t timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            timestamp |= (static_cast<uint64_t>(data[4 + i]) << (i * 8));
        }
        
        analysis += "Packet Header:\n";
        analysis += "  Sequence: " + std::to_string(sequence) + "\n";
        analysis += "  Timestamp: " + std::to_string(timestamp) + "\n\n";
        
        offset = 12;
        int msg_count = 0;
        bool has_order_entries = false;
        bool proper_alignment = true;
        
        while (offset + 2 <= length) {
            uint16_t msg_size = data[offset] | (data[offset + 1] << 8);
            offset += 2;
            
            if (msg_size == 0) break; // End of messages
            if (offset + msg_size > length) break;
            
            msg_count++;
            analysis += "Message " + std::to_string(msg_count) + ":\n";
            analysis += "  Size: " + std::to_string(msg_size) + " bytes\n";
            
            // Check alignment
            if (msg_size % 8 != 0) {
                proper_alignment = false;
                analysis += "  ❌ NOT 8-byte aligned\n";
            } else {
                analysis += "  ✅ 8-byte aligned\n";
            }
            
            // Parse SBE header if possible
            if (msg_size >= 8) {
                try {
                    MessageHeader header;
                    header.wrap(const_cast<char*>(reinterpret_cast<const char*>(data)), 
                               offset, 0, length);
                    
                    analysis += "  Template ID: " + std::to_string(header.templateId()) + "\n";
                    analysis += "  Block Length: " + std::to_string(header.blockLength()) + "\n";
                    
                    // Check for OrderID entries if Template 46
                    if (header.templateId() == 46) {
                        try {
                            MDIncrementalRefreshBook46 decoder;
                            decoder.wrapForDecode(
                                const_cast<char*>(reinterpret_cast<const char*>(data)),
                                offset + header.encodedLength(),
                                header.blockLength(),
                                header.version(),
                                length);
                            
                            auto& md_entries = decoder.noMDEntries();
                            auto& order_entries = decoder.noOrderIDEntries();
                            
                            analysis += "  MD Entries: " + std::to_string(md_entries.count()) + "\n";
                            analysis += "  Order Entries: " + std::to_string(order_entries.count()) + "\n";
                            
                            if (order_entries.count() > 0) {
                                has_order_entries = true;
                                analysis += "  ✅ HAS OrderID entries (production format)\n";
                            } else {
                                analysis += "  ❌ Missing OrderID entries\n";
                            }
                            
                        } catch (...) {
                            analysis += "  Error parsing message content\n";
                        }
                    }
                    
                } catch (...) {
                    analysis += "  Error parsing SBE header\n";
                }
            }
            
            offset += msg_size;
            analysis += "\n";
        }
        
        // Summary for mock server debugging
        analysis += "=== MOCK SERVER DEBUG SUMMARY ===\n";
        analysis += "Total messages: " + std::to_string(msg_count) + "\n";
        
        if (has_order_entries) {
            analysis += "✅ Real CME data includes OrderID entries\n";
            analysis += "   → Mock server MUST implement NoOrderIDEntries group\n";
        } else {
            analysis += "❌ No OrderID entries found\n";
            analysis += "   → Check if this is incomplete data\n";
        }
        
        if (proper_alignment) {
            analysis += "✅ All messages are 8-byte aligned\n";
            analysis += "   → Mock server alignment implementation is correct\n";
        } else {
            analysis += "❌ Some messages not 8-byte aligned\n";
            analysis += "   → Fix mock server alignment calculation\n";
        }
        
        analysis += "\nACTION ITEMS for mock server:\n";
        analysis += "1. Ensure OrderID entries are populated with realistic data\n";
        analysis += "2. Verify 8-byte message alignment: ((length + 7) / 8) * 8\n";
        analysis += "3. Match exact message sizes from this real data\n";
        analysis += "4. Include proper MD entries with price levels\n";
        
    } catch (const std::exception& e) {
        analysis += "Analysis error: " + std::string(e.what()) + "\n";
    }
    
    return analysis;
}

int main(int argc, char* argv[]) {
    std::cout << "CME Data Debug Tool for Mock Server" << std::endl;
    std::cout << "====================================" << std::endl;
    
    if (argc < 2) {
        std::cout << "\nUsage: " << argv[0] << " <hex_string>" << std::endl;
        std::cout << "\nExample:" << std::endl;
        std::cout << "  " << argv[0] << " \"01000000102030405060708058000b002e00010d00...\"" << std::endl;
        std::cout << "\nThis tool analyzes actual CME packet data to identify" << std::endl;
        std::cout << "what needs to be fixed in the mock server implementation." << std::endl;
        return 1;
    }
    
    // Parse hex string
    std::string hex = argv[1];
    // Remove spaces and common separators
    hex.erase(std::remove_if(hex.begin(), hex.end(), 
              [](char c) { return c == ' ' || c == ':' || c == '-'; }), hex.end());
    
    if (hex.length() % 2 != 0) {
        std::cerr << "Invalid hex string (odd length)" << std::endl;
        return 1;
    }
    
    std::vector<uint8_t> packet_data;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
        packet_data.push_back(byte);
    }
    
    std::cout << "\nAnalyzing " << packet_data.size() << " bytes of actual CME data...\n" << std::endl;
    
    // Analyze the data
    std::string analysis = analyze_for_mock_server_debug(packet_data.data(), packet_data.size());
    std::cout << analysis << std::endl;
    
    return 0;
}