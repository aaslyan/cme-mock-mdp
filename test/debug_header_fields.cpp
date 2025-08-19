#include "messages/sbe_decoder.h"
#include "messages/sbe_encoder.h"
#include "network/udp_receiver.h"
#include "utils/logger.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace cme_mock;

void printHexDump(const std::vector<uint8_t>& data, const std::string& label) {
    std::cout << "\n" << label << " (" << data.size() << " bytes):" << std::endl;
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
}

void analyzeRawHeader(const std::vector<uint8_t>& data) {
    std::cout << "\n=== RAW HEADER ANALYSIS ===" << std::endl;
    
    if (data.size() < 8) {
        std::cout << "Data too small for any header analysis" << std::endl;
        return;
    }
    
    // Try different header interpretations
    std::cout << "\n1. First 8 bytes as SBE MessageHeader (little-endian):" << std::endl;
    uint16_t block_length = data[0] | (data[1] << 8);
    uint16_t template_id = data[2] | (data[3] << 8);
    uint16_t schema_id = data[4] | (data[5] << 8);
    uint16_t version = data[6] | (data[7] << 8);
    
    std::cout << "  Block Length: " << block_length << std::endl;
    std::cout << "  Template ID: " << template_id << std::endl;
    std::cout << "  Schema ID: " << schema_id << std::endl;
    std::cout << "  Version: " << version << std::endl;
    
    if (data.size() >= 12) {
        std::cout << "\n2. First 12 bytes as MDP Packet Header:" << std::endl;
        uint32_t sequence = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        uint64_t timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            timestamp |= static_cast<uint64_t>(data[4 + i]) << (i * 8);
        }
        
        std::cout << "  Sequence: " << sequence << std::endl;
        std::cout << "  Timestamp: " << timestamp << std::endl;
        
        if (data.size() >= 14) {
            uint16_t msg_count = data[12] | (data[13] << 8);
            std::cout << "  Msg Count: " << msg_count << std::endl;
        }
    }
    
    if (data.size() >= 20) {
        std::cout << "\n3. Bytes 12-19 as SBE MessageHeader:" << std::endl;
        uint16_t block_length2 = data[12] | (data[13] << 8);
        uint16_t template_id2 = data[14] | (data[15] << 8);
        uint16_t schema_id2 = data[16] | (data[17] << 8);
        uint16_t version2 = data[18] | (data[19] << 8);
        
        std::cout << "  Block Length: " << block_length2 << std::endl;
        std::cout << "  Template ID: " << template_id2 << std::endl;
        std::cout << "  Schema ID: " << schema_id2 << std::endl;
        std::cout << "  Version: " << version2 << std::endl;
    }
    
    if (data.size() >= 22) {
        std::cout << "\n4. Bytes 14-21 as SBE MessageHeader:" << std::endl;
        uint16_t block_length3 = data[14] | (data[15] << 8);
        uint16_t template_id3 = data[16] | (data[17] << 8);
        uint16_t schema_id3 = data[18] | (data[19] << 8);
        uint16_t version3 = data[20] | (data[21] << 8);
        
        std::cout << "  Block Length: " << block_length3 << std::endl;
        std::cout << "  Template ID: " << template_id3 << std::endl;
        std::cout << "  Schema ID: " << schema_id3 << std::endl;
        std::cout << "  Version: " << version3 << std::endl;
    }
}

void analyzeWithDecoder(const std::vector<uint8_t>& data) {
    std::cout << "\n=== SBE DECODER ANALYSIS ===" << std::endl;
    
    SBEDecoder decoder(data);
    
    // Try to decode packet header first
    uint32_t seq_num;
    uint64_t sending_time;
    
    std::cout << "\n1. Trying to decode as complete MDP packet:" << std::endl;
    if (MDPMessageDecoder::decode_packet_header(decoder, seq_num, sending_time)) {
        std::cout << "  Packet Header decoded successfully:" << std::endl;
        std::cout << "    Sequence: " << seq_num << std::endl;
        std::cout << "    Sending Time: " << sending_time << std::endl;
        
        // Now try message header
        uint16_t block_length, schema_id, version;
        uint16_t template_id = MDPMessageDecoder::decode_message_header(
            decoder, block_length, schema_id, version);
            
        std::cout << "  Message Header:" << std::endl;
        std::cout << "    Template ID: " << template_id << std::endl;
        std::cout << "    Schema ID: " << schema_id << std::endl;
        std::cout << "    Version: " << version << std::endl;
        std::cout << "    Block Length: " << block_length << std::endl;
        
    } else {
        std::cout << "  Failed to decode packet header" << std::endl;
        
        // Try decoding as just SBE message
        std::cout << "\n2. Trying to decode as pure SBE message:" << std::endl;
        decoder.reset(data);
        uint16_t block_length, schema_id, version;
        uint16_t template_id = MDPMessageDecoder::decode_message_header(
            decoder, block_length, schema_id, version);
            
        std::cout << "  SBE Message Header:" << std::endl;
        std::cout << "    Template ID: " << template_id << std::endl;
        std::cout << "    Schema ID: " << schema_id << std::endl;
        std::cout << "    Version: " << version << std::endl;
        std::cout << "    Block Length: " << block_length << std::endl;
    }
}

void analyzeMessage(const std::vector<uint8_t>& data, int msg_num) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "MESSAGE " << msg_num << " ANALYSIS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    printHexDump(data, "Raw Message Data");
    analyzeRawHeader(data);
    analyzeWithDecoder(data);
}

int main() {
    std::cout << "CME Header Field Debug Tool" << std::endl;
    std::cout << "===========================" << std::endl;
    
    Logger::instance().set_level(LogLevel::INFO);
    
    // Listen to incremental feed
    UDPReceiver incremental("127.0.0.1", 14310);
    if (!incremental.initialize()) {
        std::cerr << "Failed to initialize incremental receiver" << std::endl;
        return 1;
    }
    
    std::cout << "Listening for incremental messages on port 14310..." << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    
    int message_count = 0;
    while (message_count < 5) { // Analyze first 5 messages
        std::vector<uint8_t> data;
        if (incremental.receive_once(data, 5000)) {
            message_count++;
            
            // Print what client received
            std::cout << "CLIENT RECEIVED (" << data.size() << " bytes): ";
            for (size_t i = 0; i < std::min(data.size(), size_t(32)); ++i) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)data[i] << " ";
            }
            std::cout << std::dec << std::endl;
            
            analyzeMessage(data, message_count);
        } else {
            std::cout << "." << std::flush;
        }
    }
    
    std::cout << "\nAnalysis complete." << std::endl;
    return 0;
}