#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>

using namespace cme_sbe;

struct CMEDebugInfo {
    // Packet level
    size_t total_packet_size;
    uint32_t sequence_number;
    uint64_t sending_time;
    int message_count;
    
    // Message level details
    struct MessageDebug {
        int message_index;
        uint16_t declared_size;
        uint16_t actual_sbe_length;
        uint16_t block_length;
        uint16_t template_id;
        uint16_t schema_id;
        uint16_t version;
        size_t padding_bytes;
        bool size_matches_content;
        
        // Content details
        int md_entries_count;
        int order_entries_count;
        uint64_t transact_time;
        
        std::vector<uint32_t> security_ids;
        std::vector<uint8_t> price_levels;
    };
    
    std::vector<MessageDebug> messages;
    
    // Comparison flags for mock server debugging
    bool has_proper_alignment;
    bool has_order_entries;
    bool matches_expected_size;
    std::string diagnosis;
};

void hexdump_section(const uint8_t* data, size_t start, size_t length, const std::string& title) {
    std::cout << "\n" << title << " (offset " << start << ", " << length << " bytes):" << std::endl;
    for (size_t i = 0; i < length; i += 16) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << (start + i) << ": ";
        
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < length) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)data[start + i + j] << " ";
            } else {
                std::cout << "   ";
            }
        }
        
        std::cout << " |";
        for (size_t j = 0; j < 16 && i + j < length; ++j) {
            char c = data[start + i + j];
            std::cout << (isprint(c) ? c : '.');
        }
        std::cout << "|" << std::dec << std::endl;
    }
}

CMEDebugInfo analyze_actual_cme_data(const uint8_t* data, size_t length) {
    CMEDebugInfo debug_info = {};
    debug_info.total_packet_size = length;
    
    std::cout << "=== CME ACTUAL DATA ANALYSIS ===" << std::endl;
    std::cout << "Total packet size: " << length << " bytes" << std::endl;
    
    if (length < 12) {
        debug_info.diagnosis = "Packet too small for CME header";
        return debug_info;
    }
    
    // Show full packet hex dump
    hexdump_section(data, 0, length, "Complete Packet");
    
    try {
        size_t offset = 0;
        
        // Parse packet header
        debug_info.sequence_number = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        debug_info.sending_time = 0;
        for (int i = 0; i < 8; ++i) {
            debug_info.sending_time |= (static_cast<uint64_t>(data[4 + i]) << (i * 8));
        }
        
        std::cout << "\nPACKET HEADER ANALYSIS:" << std::endl;
        std::cout << "  Sequence Number: " << debug_info.sequence_number << " (0x" << std::hex << debug_info.sequence_number << std::dec << ")" << std::endl;
        std::cout << "  Sending Time: " << debug_info.sending_time << " (0x" << std::hex << debug_info.sending_time << std::dec << ")" << std::endl;
        
        offset = 12;
        debug_info.message_count = 0;
        
        // Analyze each message
        while (offset < length) {
            CMEDebugInfo::MessageDebug msg_debug = {};
            msg_debug.message_index = debug_info.message_count + 1;
            
            if (offset + 2 > length) {
                std::cout << "\nERROR: Incomplete message size field at offset " << offset << std::endl;
                break;
            }
            
            // Read message size
            msg_debug.declared_size = data[offset] | (data[offset + 1] << 8);
            offset += 2;
            
            std::cout << "\n=== MESSAGE " << msg_debug.message_index << " ANALYSIS ===" << std::endl;
            std::cout << "Declared Size: " << msg_debug.declared_size << " bytes" << std::endl;
            std::cout << "Message starts at offset: " << offset << std::endl;
            
            if (offset + msg_debug.declared_size > length) {
                std::cout << "ERROR: Message extends beyond packet boundary" << std::endl;
                break;
            }
            
            // Show message hex dump
            hexdump_section(data, offset, std::min((size_t)msg_debug.declared_size, (size_t)64), "Message Content (first 64 bytes)");
            
            // Analyze SBE header
            if (msg_debug.declared_size >= 8) {
                try {
                    MessageHeader header;
                    header.wrap(const_cast<char*>(reinterpret_cast<const char*>(data)), 
                               offset, 0, length);
                    
                    msg_debug.block_length = header.blockLength();
                    msg_debug.template_id = header.templateId();
                    msg_debug.schema_id = header.schemaId();
                    msg_debug.version = header.version();
                    
                    std::cout << "SBE Header Analysis:" << std::endl;
                    std::cout << "  Block Length: " << msg_debug.block_length << std::endl;
                    std::cout << "  Template ID: " << msg_debug.template_id << std::endl;
                    std::cout << "  Schema ID: " << msg_debug.schema_id << std::endl;
                    std::cout << "  Version: " << msg_debug.version << std::endl;
                    
                    // Calculate actual SBE content length
                    size_t sbe_header_size = 8;
                    size_t min_message_size = sbe_header_size + msg_debug.block_length;
                    
                    std::cout << "Size Analysis:" << std::endl;
                    std::cout << "  SBE Header: 8 bytes" << std::endl;
                    std::cout << "  Block Length: " << msg_debug.block_length << " bytes" << std::endl;
                    std::cout << "  Minimum SBE size: " << min_message_size << " bytes" << std::endl;
                    std::cout << "  Declared message size: " << msg_debug.declared_size << " bytes" << std::endl;
                    
                    if (msg_debug.declared_size > min_message_size) {
                        msg_debug.padding_bytes = msg_debug.declared_size - min_message_size;
                        std::cout << "  Padding/Groups: " << msg_debug.padding_bytes << " bytes" << std::endl;
                        
                        // Check if padding follows 8-byte alignment
                        if (msg_debug.declared_size % 8 == 0) {
                            std::cout << "  ✅ Message is 8-byte aligned" << std::endl;
                            debug_info.has_proper_alignment = true;
                        } else {
                            std::cout << "  ❌ Message is NOT 8-byte aligned" << std::endl;
                        }
                    }
                    
                    // Try to parse content if it's Template ID 46
                    if (msg_debug.template_id == 46) {
                        try {
                            MDIncrementalRefreshBook46 decoder;
                            decoder.wrapForDecode(
                                const_cast<char*>(reinterpret_cast<const char*>(data)),
                                offset + header.encodedLength(),
                                header.blockLength(),
                                header.version(),
                                length);
                            
                            msg_debug.transact_time = decoder.transactTime();
                            std::cout << "Content Analysis:" << std::endl;
                            std::cout << "  Transaction Time: " << msg_debug.transact_time << std::endl;
                            
                            // Analyze NoMDEntries
                            auto& md_entries = decoder.noMDEntries();
                            msg_debug.md_entries_count = md_entries.count();
                            std::cout << "  MD Entries Count: " << msg_debug.md_entries_count << std::endl;
                            
                            while (md_entries.hasNext()) {
                                md_entries.next();
                                msg_debug.security_ids.push_back(md_entries.securityID());
                                msg_debug.price_levels.push_back(md_entries.mDPriceLevel());
                            }
                            
                            // Analyze NoOrderIDEntries  
                            auto& order_entries = decoder.noOrderIDEntries();
                            msg_debug.order_entries_count = order_entries.count();
                            std::cout << "  Order Entries Count: " << msg_debug.order_entries_count << std::endl;
                            
                            if (msg_debug.order_entries_count > 0) {
                                debug_info.has_order_entries = true;
                                std::cout << "  ✅ HAS OrderID entries (production format)" << std::endl;
                                
                                int order_count = 0;
                                while (order_entries.hasNext() && order_count < 3) { // Show first 3
                                    order_entries.next();
                                    order_count++;
                                    std::cout << "    Order " << order_count << ": ID=" << order_entries.orderID() 
                                             << ", Priority=" << order_entries.mDOrderPriority() << std::endl;
                                }
                            } else {
                                std::cout << "  ❌ NO OrderID entries" << std::endl;
                            }
                            
                        } catch (const std::exception& e) {
                            std::cout << "  Error parsing message content: " << e.what() << std::endl;
                        }
                    }
                    
                } catch (const std::exception& e) {
                    std::cout << "Error parsing SBE header: " << e.what() << std::endl;
                }
            }
            
            debug_info.messages.push_back(msg_debug);
            debug_info.message_count++;
            
            // Advance to next message
            offset += msg_debug.declared_size;
            std::cout << "Advanced to offset: " << offset << std::endl;
        }
        
        // Generate diagnosis for mock server debugging
        std::cout << "\n=== MOCK SERVER DEBUG DIAGNOSIS ===" << std::endl;
        
        std::string diagnosis = "ACTUAL CME DATA ANALYSIS:\n";
        diagnosis += "- Total packet size: " + std::to_string(debug_info.total_packet_size) + " bytes\n";
        diagnosis += "- Message count: " + std::to_string(debug_info.message_count) + "\n";
        
        for (const auto& msg : debug_info.messages) {
            diagnosis += "- Message " + std::to_string(msg.message_index) + ": " + 
                        std::to_string(msg.declared_size) + " bytes, Template " + 
                        std::to_string(msg.template_id) + ", Block " + 
                        std::to_string(msg.block_length) + "\n";
            diagnosis += "  MD Entries: " + std::to_string(msg.md_entries_count) + 
                        ", Order Entries: " + std::to_string(msg.order_entries_count) + "\n";
        }
        
        if (debug_info.has_order_entries) {
            diagnosis += "\n✅ CRITICAL: Real CME data HAS OrderID entries\n";
            diagnosis += "   Mock server MUST include NoOrderIDEntries group\n";
        }
        
        if (debug_info.has_proper_alignment) {
            diagnosis += "✅ Real CME data uses 8-byte alignment\n";
        }
        
        // Specific recommendations
        diagnosis += "\nMOCK SERVER FIXES NEEDED:\n";
        if (!debug_info.has_order_entries) {
            diagnosis += "1. Add OrderID entries to match production format\n";
        }
        diagnosis += "2. Ensure 8-byte message alignment\n";
        diagnosis += "3. Match exact message sizes from real data\n";
        
        debug_info.diagnosis = diagnosis;
        std::cout << diagnosis << std::endl;
        
    } catch (const std::exception& e) {
        debug_info.diagnosis = "Analysis failed: " + std::string(e.what());
        std::cout << "Analysis error: " << e.what() << std::endl;
    }
    
    return debug_info;
}

int main(int argc, char* argv[]) {
    std::cout << "CME Actual Data Analyzer" << std::endl;
    std::cout << "========================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <hex_string_or_file>" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << " \"01002030405060708090a0b0c0...\"" << std::endl;
        std::cout << "  " << argv[0] << " packet.bin" << std::endl;
        return 1;
    }
    
    std::vector<uint8_t> packet_data;
    
    // Check if argument is a file or hex string
    std::string input = argv[1];
    if (input.find(".") != std::string::npos) {
        // Assume it's a file
        std::ifstream file(input, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << input << std::endl;
            return 1;
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        packet_data.resize(size);
        file.read(reinterpret_cast<char*>(packet_data.data()), size);
        
        std::cout << "Loaded " << size << " bytes from file: " << input << std::endl;
    } else {
        // Parse hex string
        std::string hex = input;
        // Remove spaces and common separators
        hex.erase(std::remove_if(hex.begin(), hex.end(), 
                  [](char c) { return c == ' ' || c == ':' || c == '-'; }), hex.end());
        
        if (hex.length() % 2 != 0) {
            std::cerr << "Invalid hex string (odd length)" << std::endl;
            return 1;
        }
        
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byte_str = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
            packet_data.push_back(byte);
        }
        
        std::cout << "Parsed " << packet_data.size() << " bytes from hex string" << std::endl;
    }
    
    if (packet_data.empty()) {
        std::cerr << "No data to analyze" << std::endl;
        return 1;
    }
    
    // Analyze the data
    CMEDebugInfo debug_info = analyze_actual_cme_data(packet_data.data(), packet_data.size());
    
    // Save analysis to file
    std::ofstream report("cme_analysis_report.txt");
    if (report.is_open()) {
        report << debug_info.diagnosis << std::endl;
        report << "\nDetailed packet info:" << std::endl;
        report << "Sequence: " << debug_info.sequence_number << std::endl;
        report << "Timestamp: " << debug_info.sending_time << std::endl;
        report << "Messages: " << debug_info.message_count << std::endl;
        
        for (const auto& msg : debug_info.messages) {
            report << "\nMessage " << msg.message_index << ":" << std::endl;
            report << "  Size: " << msg.declared_size << std::endl;
            report << "  Template: " << msg.template_id << std::endl;
            report << "  Block Length: " << msg.block_length << std::endl;
            report << "  MD Entries: " << msg.md_entries_count << std::endl;
            report << "  Order Entries: " << msg.order_entries_count << std::endl;
        }
        
        report.close();
        std::cout << "\nAnalysis report saved to: cme_analysis_report.txt" << std::endl;
    }
    
    return 0;
}