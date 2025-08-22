#include "utils/packet_verifier.h"
#include "utils/hexdump.h"
#include "utils/logger.h"
#include <iostream>

namespace cme_mock {

PacketVerificationResult PacketVerifier::verify_packet(const std::vector<uint8_t>& packet)
{
    PacketVerificationResult result;
    result.is_valid = false;
    result.error_message = "";

    size_t offset = 0;

    try {
        // Step 1: Verify minimum packet size (header + at least one message)
        if (packet.size() < 14) { // 12 header + 2 size + 0 message minimum
            result.error_message = "Packet too small: " + std::to_string(packet.size()) + " bytes";
            return result;
        }

        // Step 2: Parse packet header (12 bytes)
        if (offset + 12 > packet.size()) {
            result.error_message = "Cannot read packet header";
            return result;
        }

        uint32_t sequence_number = packet[offset] | (packet[offset + 1] << 8) | (packet[offset + 2] << 16) | (packet[offset + 3] << 24);

        uint64_t sending_time = 0;
        for (int i = 0; i < 8; ++i) {
            sending_time |= (static_cast<uint64_t>(packet[offset + 4 + i]) << (i * 8));
        }

        result.sequence_number = sequence_number;
        result.sending_time = sending_time;
        offset += 12;

        LOG_INFO("Packet header: seq=" + std::to_string(sequence_number) + ", time=" + std::to_string(sending_time));

        // Step 3: Walk through messages
        int message_count = 0;
        while (offset < packet.size()) {
            // Check if we have at least 2 bytes for message size
            if (offset + 2 > packet.size()) {
                result.error_message = "Incomplete message size field at offset " + std::to_string(offset);
                return result;
            }

            // Read message size (2 bytes, little-endian)
            uint16_t message_size = packet[offset] | (packet[offset + 1] << 8);
            offset += 2;

            LOG_INFO("Message " + std::to_string(message_count + 1) + ": size=" + std::to_string(message_size) + " bytes at offset " + std::to_string(offset - 2));

            // Verify message size is reasonable
            if (message_size == 0) {
                result.error_message = "Zero-length message at offset " + std::to_string(offset - 2);
                return result;
            }

            if (message_size > 1024) { // Reasonable max size
                result.error_message = "Message too large: " + std::to_string(message_size) + " bytes at offset " + std::to_string(offset - 2);
                return result;
            }

            // Check if we have enough data for the message
            if (offset + message_size > packet.size()) {
                result.error_message = "Incomplete message: need " + std::to_string(message_size) + " bytes but only " + std::to_string(packet.size() - offset) + " available at offset " + std::to_string(offset);
                return result;
            }

            // Parse SBE message header (first 8 bytes of message)
            if (message_size < 8) {
                result.error_message = "Message too small for SBE header: " + std::to_string(message_size) + " bytes";
                return result;
            }

            uint16_t block_length = packet[offset] | (packet[offset + 1] << 8);
            uint16_t template_id = packet[offset + 2] | (packet[offset + 3] << 8);
            uint16_t schema_id = packet[offset + 4] | (packet[offset + 5] << 8);
            uint16_t version = packet[offset + 6] | (packet[offset + 7] << 8);

            MessageInfo msg_info;
            msg_info.size_field_offset = offset - 2;
            msg_info.message_offset = offset;
            msg_info.size = message_size;
            msg_info.block_length = block_length;
            msg_info.template_id = template_id;
            msg_info.schema_id = schema_id;
            msg_info.version = version;

            result.messages.push_back(msg_info);

            LOG_INFO("  SBE header: blockLen=" + std::to_string(block_length) + ", templateId=" + std::to_string(template_id) + ", schemaId=" + std::to_string(schema_id) + ", version=" + std::to_string(version));

            // CRITICAL: Advance by exactly the message size (this is what client does)
            offset += message_size;
            message_count++;

            LOG_INFO("  Advanced to offset " + std::to_string(offset) + " (remaining: " + std::to_string(packet.size() - offset) + " bytes)");
        }

        // Step 4: Verify we consumed exactly the entire packet
        if (offset != packet.size()) {
            result.error_message = "Packet parsing error: ended at offset " + std::to_string(offset) + " but packet size is " + std::to_string(packet.size());
            return result;
        }

        result.total_messages = message_count;
        result.is_valid = true;
        result.error_message = "Packet valid: " + std::to_string(message_count) + " messages";

        LOG_INFO("✅ Packet verification successful: " + std::to_string(message_count) + " messages");

    } catch (const std::exception& e) {
        result.error_message = "Exception during verification: " + std::string(e.what());
        return result;
    }

    return result;
}

void PacketVerifier::print_verification_result(const PacketVerificationResult& result)
{
    std::cout << "=== Packet Verification Result ===" << std::endl;
    std::cout << "Valid: " << (result.is_valid ? "YES" : "NO") << std::endl;
    std::cout << "Status: " << result.error_message << std::endl;

    if (result.is_valid) {
        std::cout << "Sequence Number: " << result.sequence_number << std::endl;
        std::cout << "Sending Time: " << result.sending_time << std::endl;
        std::cout << "Total Messages: " << result.total_messages << std::endl;

        for (size_t i = 0; i < result.messages.size(); ++i) {
            const auto& msg = result.messages[i];
            std::cout << std::endl;
            std::cout << "Message " << (i + 1) << ":" << std::endl;
            std::cout << "  Size field offset: " << msg.size_field_offset << std::endl;
            std::cout << "  Message offset: " << msg.message_offset << std::endl;
            std::cout << "  Size: " << msg.size << " bytes" << std::endl;
            std::cout << "  Block Length: " << msg.block_length << std::endl;
            std::cout << "  Template ID: " << msg.template_id << std::endl;
            std::cout << "  Schema ID: " << msg.schema_id << std::endl;
            std::cout << "  Version: " << msg.version << std::endl;
        }
    }
    std::cout << "=================================" << std::endl;
}

bool PacketVerifier::verify_and_log(const std::vector<uint8_t>& packet, const std::string& description)
{
    std::cout << "\n=== Verifying: " << description << " ===" << std::endl;
    std::cout << "Packet size: " << packet.size() << " bytes" << std::endl;

    // Show hex dump for debugging
    hexdump_compact(packet, "Packet data");

    auto result = verify_packet(packet);
    print_verification_result(result);

    return result.is_valid;
}

} // namespace cme_mock