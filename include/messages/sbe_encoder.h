#pragma once

#include "mdp_messages.h"
#include <cstdint>
#include <cstring>
#include <vector>

namespace cme_mock {

// SBE field encoding helpers
class SBEEncoder {
public:
    SBEEncoder()
        : buffer_(1024)
        , offset_(0)
    {
    }

    // Get encoded data
    std::vector<uint8_t> get_buffer() const
    {
        return std::vector<uint8_t>(buffer_.begin(), buffer_.begin() + offset_);
    }

    size_t get_offset() const { return offset_; }
    void reset() { offset_ = 0; }

    // Basic type encoders
    void encode_uint8(uint8_t value);
    void encode_uint16(uint16_t value);
    void encode_uint32(uint32_t value);
    void encode_uint64(uint64_t value);
    void encode_int32(int32_t value);
    void encode_int64(int64_t value);
    void encode_char(char value);
    void encode_string(const char* str, size_t length);

    // MDP specific encoders
    void encode_price(double price, uint8_t decimals);
    void encode_quantity(int32_t quantity);

private:
    std::vector<uint8_t> buffer_;
    size_t offset_;

    void ensure_capacity(size_t required);
};

// MDP 3.0 Message Encoder
class MDPMessageEncoder {
public:
    // Encode packet header
    static std::vector<uint8_t> encode_packet_header(
        uint32_t sequence_number,
        uint64_t sending_time,
        uint16_t msg_count = 1);

    // Encode snapshot message
    static std::vector<uint8_t> encode_snapshot_full_refresh(
        const SnapshotFullRefresh& snapshot);

    // Encode incremental refresh message
    static std::vector<uint8_t> encode_incremental_refresh(
        const IncrementalRefresh& incremental);

    // Encode admin messages
    static std::vector<uint8_t> encode_channel_reset(
        uint64_t transaction_time);

    static std::vector<uint8_t> encode_heartbeat(
        uint64_t timestamp);

private:
    // Message header encoding
    static void encode_message_header(
        SBEEncoder& encoder,
        uint16_t template_id,
        uint16_t schema_id,
        uint16_t version,
        uint16_t block_length);

    // Snapshot helpers
    static void encode_snapshot_entry(
        SBEEncoder& encoder,
        const MDSnapshotEntry& entry);

    // Incremental helpers
    static void encode_price_level(
        SBEEncoder& encoder,
        const MDPriceLevel& level);

    static void encode_trade(
        SBEEncoder& encoder,
        const MDTrade& trade);
};

// Constants for MDP 3.0
// Schema Version 9 for traditional channels (310, etc.)
constexpr uint16_t MDP_SCHEMA_ID = 1;
constexpr uint16_t MDP_VERSION = 9;

// Schema Version 13 for FX Spot+ Channel 330
constexpr uint16_t MDP_SCHEMA_ID_FX = 1;
constexpr uint16_t MDP_VERSION_FX = 13;

// Template IDs for Schema Version 9 (traditional channels)
constexpr uint16_t TEMPLATE_CHANNEL_RESET = 4;
constexpr uint16_t TEMPLATE_ADMIN_HEARTBEAT = 12;
constexpr uint16_t TEMPLATE_INCREMENTAL_REFRESH_BOOK = 46;
constexpr uint16_t TEMPLATE_INCREMENTAL_REFRESH_TRADE = 48;
constexpr uint16_t TEMPLATE_SNAPSHOT_FULL_REFRESH = 52;

// Template IDs for Schema Version 13 (FX Spot+ Channel 330)
// Based on official CME templates.xml v13: same template IDs, different schema version
constexpr uint16_t TEMPLATE_FX_INCREMENTAL_REFRESH_BOOK = 46;
constexpr uint16_t TEMPLATE_FX_INCREMENTAL_REFRESH_TRADE = 48;
constexpr uint16_t TEMPLATE_FX_SNAPSHOT_FULL_REFRESH = 52;

// Block lengths (simplified, actual lengths vary)
constexpr uint16_t BLOCK_LENGTH_SNAPSHOT = 44;
constexpr uint16_t BLOCK_LENGTH_INCREMENTAL = 32;

} // namespace cme_mock