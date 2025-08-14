#include <iostream>
#include <iomanip>
#include "network/udp_receiver.h"
#include "messages/sbe_decoder.h"
#include "messages/sbe_encoder.h"
#include "utils/logger.h"

void debug_incremental_decode(const std::vector<uint8_t>& data) {
    std::cout << "\n=== Detailed Incremental Debug ===" << std::endl;
    
    cme_mock::SBEDecoder decoder(data);
    
    // Decode packet header
    uint32_t seq_num;
    uint64_t sending_time;
    if (!cme_mock::MDPMessageDecoder::decode_packet_header(decoder, seq_num, sending_time)) {
        std::cout << "❌ Failed to decode packet header" << std::endl;
        return;
    }
    std::cout << "✓ Packet Header - Seq: " << seq_num << std::endl;
    
    // Decode message header
    uint16_t block_length, schema_id, version;
    uint16_t template_id = cme_mock::MDPMessageDecoder::decode_message_header(
        decoder, block_length, schema_id, version);
    
    std::cout << "✓ Message Header - Template: " << template_id 
              << ", Block Length: " << block_length << std::endl;
    
    if (template_id != cme_mock::TEMPLATE_INCREMENTAL_REFRESH_BOOK) {
        std::cout << "❌ Wrong template ID, expected " 
                  << cme_mock::TEMPLATE_INCREMENTAL_REFRESH_BOOK << std::endl;
        return;
    }
    
    // Manual decode to debug step by step
    std::cout << "Decoder position: " << decoder.get_offset() 
              << ", remaining: " << decoder.remaining() << std::endl;
    
    // Check if we can read transact_time (8 bytes)
    if (!decoder.can_read(8)) {
        std::cout << "❌ Cannot read transact_time (need 8 bytes)" << std::endl;
        return;
    }
    
    uint64_t transact_time = decoder.decode_uint64();
    std::cout << "✓ Transact time: " << transact_time << std::endl;
    
    // Check if we can read group header (4 bytes)
    if (!decoder.can_read(4)) {
        std::cout << "❌ Cannot read price level group header (need 4 bytes)" << std::endl;
        return;
    }
    
    uint16_t num_levels = decoder.decode_uint16();
    uint16_t level_length = decoder.decode_uint16();
    std::cout << "✓ Price levels group - Count: " << num_levels 
              << ", Length: " << level_length << std::endl;
    
    // Try to decode price levels
    for (uint16_t i = 0; i < num_levels; ++i) {
        std::cout << "Decoding price level " << i+1 << "/" << num_levels << std::endl;
        std::cout << "  Decoder position: " << decoder.get_offset() 
                  << ", remaining: " << decoder.remaining() << std::endl;
        
        if (!decoder.can_read(29)) {
            std::cout << "  ❌ Cannot read price level (need 29 bytes)" << std::endl;
            return;
        }
        
        uint8_t update_action = decoder.decode_uint8();
        char entry_type = decoder.decode_char();
        decoder.skip(2); // padding
        uint32_t security_id = decoder.decode_uint32();
        uint32_t rpt_seq = decoder.decode_uint32();
        int64_t price = decoder.decode_int64();
        int32_t quantity = decoder.decode_int32();
        uint32_t num_orders = decoder.decode_uint32();
        uint8_t price_level = decoder.decode_uint8();
        // No additional padding - 29 bytes total
        
        std::cout << "  ✓ Level " << i+1 << " - Action: " << (int)update_action
                  << ", Type: " << entry_type << ", Security: " << security_id
                  << ", Price: " << (price/100.0) << ", Qty: " << quantity << std::endl;
    }
    
    // Check trade group
    if (!decoder.can_read(4)) {
        std::cout << "❌ Cannot read trade group header (need 4 bytes)" << std::endl;
        return;
    }
    
    uint16_t num_trades = decoder.decode_uint16();
    uint16_t trade_length = decoder.decode_uint16();
    std::cout << "✓ Trades group - Count: " << num_trades 
              << ", Length: " << trade_length << std::endl;
    
    std::cout << "✓ Incremental decode successful!" << std::endl;
}

int main() {
    std::cout << "Incremental Message Debug Tool" << std::endl;
    std::cout << "==============================" << std::endl;
    
    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);
    
    // Listen to incremental feed
    cme_mock::UDPReceiver incremental("127.0.0.1", 14310);
    if (!incremental.initialize()) {
        std::cerr << "Failed to initialize incremental receiver" << std::endl;
        return 1;
    }
    
    std::cout << "Listening for incremental messages on port 14310..." << std::endl;
    
    int message_count = 0;
    while (message_count < 3) {  // Debug first 3 messages
        std::vector<uint8_t> data;
        if (incremental.receive_once(data, 5000)) {
            message_count++;
            std::cout << "\n--- Message " << message_count << " ---";
            debug_incremental_decode(data);
        } else {
            std::cout << "." << std::flush;
        }
    }
    
    return 0;
}