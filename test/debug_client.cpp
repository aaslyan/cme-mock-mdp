#include <iostream>
#include <iomanip>
#include "network/udp_receiver.h"
#include "messages/sbe_decoder.h"
#include "messages/sbe_encoder.h"
#include "utils/logger.h"

void analyze_message(const std::vector<uint8_t>& data) {
    std::cout << "\n=== Message Analysis (" << data.size() << " bytes) ===" << std::endl;
    
    // Print hex dump of first 32 bytes
    std::cout << "Hex dump: ";
    for (size_t i = 0; i < std::min(data.size(), size_t(32)); ++i) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
    
    cme_mock::SBEDecoder decoder(data);
    
    // Try to decode packet header
    uint32_t seq_num;
    uint64_t sending_time;
    if (cme_mock::MDPMessageDecoder::decode_packet_header(decoder, seq_num, sending_time)) {
        std::cout << "Packet Header - Seq: " << seq_num << ", Time: " << sending_time << std::endl;
        
        // Try to decode message header
        uint16_t block_length, schema_id, version;
        uint16_t template_id = cme_mock::MDPMessageDecoder::decode_message_header(
            decoder, block_length, schema_id, version);
        
        std::cout << "Message Header - Template: " << template_id 
                  << ", Schema: " << schema_id << ", Version: " << version 
                  << ", Block Length: " << block_length << std::endl;
        
        // Identify message type
        if (template_id == cme_mock::TEMPLATE_SNAPSHOT_FULL_REFRESH) {
            std::cout << "Message Type: SNAPSHOT_FULL_REFRESH" << std::endl;
        } else if (template_id == cme_mock::TEMPLATE_INCREMENTAL_REFRESH_BOOK) {
            std::cout << "Message Type: INCREMENTAL_REFRESH_BOOK" << std::endl;
        } else if (template_id == cme_mock::TEMPLATE_ADMIN_HEARTBEAT) {
            std::cout << "Message Type: ADMIN_HEARTBEAT" << std::endl;
        } else if (template_id == cme_mock::TEMPLATE_CHANNEL_RESET) {
            std::cout << "Message Type: CHANNEL_RESET" << std::endl;
        } else {
            std::cout << "Message Type: UNKNOWN (ID: " << template_id << ")" << std::endl;
        }
    } else {
        std::cout << "Failed to decode packet header" << std::endl;
    }
}

int main() {
    std::cout << "CME Message Debug Tool" << std::endl;
    std::cout << "======================" << std::endl;
    
    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);
    
    // Listen to incremental feed
    cme_mock::UDPReceiver incremental("127.0.0.1", 14310);
    if (!incremental.initialize()) {
        std::cerr << "Failed to initialize incremental receiver" << std::endl;
        return 1;
    }
    
    std::cout << "Listening for incremental messages on port 14310..." << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    
    int message_count = 0;
    while (message_count < 10) {  // Just analyze first 10 messages
        std::vector<uint8_t> data;
        if (incremental.receive_once(data, 5000)) {
            message_count++;
            std::cout << "\n--- Message " << message_count << " ---";
            analyze_message(data);
        } else {
            std::cout << "." << std::flush;
        }
    }
    
    std::cout << "\nAnalysis complete." << std::endl;
    return 0;
}