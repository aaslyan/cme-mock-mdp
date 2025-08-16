#include "messages/sbe_decoder.h"
#include "messages/sbe_encoder.h"
#include "network/udp_receiver.h"
#include "utils/logger.h"
#include <iomanip>
#include <iostream>

void analyze_message(const std::vector<uint8_t>& data)
{
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

int main()
{
    std::cout << "CME Snapshot Debug Tool" << std::endl;
    std::cout << "=======================" << std::endl;

    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);

    // Listen to snapshot feed
    cme_mock::UDPReceiver snapshot("127.0.0.1", 14320);
    if (!snapshot.initialize()) {
        std::cerr << "Failed to initialize snapshot receiver" << std::endl;
        return 1;
    }

    std::cout << "Listening for snapshot messages on port 14320..." << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;

    int message_count = 0;
    int snapshot_count = 0;
    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        std::vector<uint8_t> data;
        if (snapshot.receive_once(data, 1000)) { // 1 second timeout
            message_count++;
            std::cout << "\n--- Message " << message_count << " ---";
            analyze_message(data);

            // Check if this is a snapshot message
            cme_mock::SBEDecoder decoder(data);
            uint32_t seq_num;
            uint64_t sending_time;
            if (cme_mock::MDPMessageDecoder::decode_packet_header(decoder, seq_num, sending_time)) {
                uint16_t block_length, schema_id, version;
                uint16_t template_id = cme_mock::MDPMessageDecoder::decode_message_header(
                    decoder, block_length, schema_id, version);
                if (template_id == cme_mock::TEMPLATE_SNAPSHOT_FULL_REFRESH) {
                    snapshot_count++;
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::steady_clock::now() - start_time)
                                       .count();
                    std::cout << "SNAPSHOT DETECTED! Total snapshots: " << snapshot_count
                              << " (elapsed time: " << elapsed << " seconds)" << std::endl;
                }
            }

            // Exit after 65 seconds or 3 snapshot cycles
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start_time)
                               .count();
            if (elapsed >= 65 || snapshot_count >= 80) { // 40 instruments * 2 cycles
                break;
            }
        } else {
            std::cout << "." << std::flush;
        }
    }

    auto total_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - start_time)
                             .count();

    std::cout << "\n\nAnalysis complete." << std::endl;
    std::cout << "Total messages received: " << message_count << std::endl;
    std::cout << "Total snapshots received: " << snapshot_count << std::endl;
    std::cout << "Total time: " << total_elapsed << " seconds" << std::endl;

    return 0;
}