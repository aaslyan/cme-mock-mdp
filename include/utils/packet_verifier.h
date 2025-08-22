#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cme_mock {

struct MessageInfo {
    size_t size_field_offset; // Offset of the 2-byte size field
    size_t message_offset; // Offset where message data starts
    uint16_t size; // Message size from size field
    uint16_t block_length; // SBE block length
    uint16_t template_id; // SBE template ID
    uint16_t schema_id; // SBE schema ID
    uint16_t version; // SBE version
};

struct PacketVerificationResult {
    bool is_valid;
    std::string error_message;
    uint32_t sequence_number;
    uint64_t sending_time;
    int total_messages;
    std::vector<MessageInfo> messages;
};

class PacketVerifier {
public:
    /**
     * Verify a CME MDP packet by walking through it exactly like a client would
     * @param packet The complete UDP packet data
     * @return Verification result with detailed information
     */
    static PacketVerificationResult verify_packet(const std::vector<uint8_t>& packet);

    /**
     * Print detailed verification results
     * @param result The verification result to print
     */
    static void print_verification_result(const PacketVerificationResult& result);

    /**
     * Convenience method: verify packet and log results
     * @param packet The packet to verify
     * @param description Description for logging
     * @return true if packet is valid
     */
    static bool verify_and_log(const std::vector<uint8_t>& packet, const std::string& description);
};

} // namespace cme_mock