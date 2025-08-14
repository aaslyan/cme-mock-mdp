#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "mdp_messages.h"

namespace cme_mock {

// SBE field decoding helpers
class SBEDecoder {
public:
    SBEDecoder(const std::vector<uint8_t>& data) : buffer_(data), offset_(0) {}
    SBEDecoder(const uint8_t* data, size_t length) : buffer_(data, data + length), offset_(0) {}
    
    // Reset for reuse
    void reset(const std::vector<uint8_t>& data) {
        buffer_ = data;
        offset_ = 0;
    }
    
    // Check if we can read more data
    bool can_read(size_t bytes) const {
        return offset_ + bytes <= buffer_.size();
    }
    
    size_t get_offset() const { return offset_; }
    size_t remaining() const { return buffer_.size() - offset_; }
    
    // Basic type decoders
    uint8_t decode_uint8();
    uint16_t decode_uint16();
    uint32_t decode_uint32();
    uint64_t decode_uint64();
    int32_t decode_int32();
    int64_t decode_int64();
    char decode_char();
    std::string decode_string(size_t length);
    
    // MDP specific decoders
    double decode_price(uint8_t decimals);
    
    // Skip bytes (for padding)
    void skip(size_t bytes);

private:
    std::vector<uint8_t> buffer_;
    size_t offset_;
};

// MDP 3.0 Message Decoder
class MDPMessageDecoder {
public:
    // Decode packet header
    static bool decode_packet_header(
        SBEDecoder& decoder,
        uint32_t& sequence_number,
        uint64_t& sending_time);
    
    // Decode message header and return template ID
    static uint16_t decode_message_header(
        SBEDecoder& decoder,
        uint16_t& block_length,
        uint16_t& schema_id,
        uint16_t& version);
    
    // Decode specific message types
    static bool decode_snapshot_full_refresh(
        SBEDecoder& decoder,
        SnapshotFullRefresh& snapshot);
    
    static bool decode_incremental_refresh(
        SBEDecoder& decoder,
        IncrementalRefresh& incremental);
    
    static bool decode_heartbeat(
        SBEDecoder& decoder,
        uint64_t& timestamp);
    
    static bool decode_channel_reset(
        SBEDecoder& decoder,
        uint64_t& timestamp);

private:
    // Helper methods
    static bool decode_snapshot_entry(
        SBEDecoder& decoder,
        MDSnapshotEntry& entry);
    
    static bool decode_price_level(
        SBEDecoder& decoder,
        MDPriceLevel& level);
    
    static bool decode_trade(
        SBEDecoder& decoder,
        MDTrade& trade);
};

} // namespace cme_mock