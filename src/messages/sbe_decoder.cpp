#include "messages/sbe_decoder.h"
#include "messages/sbe_encoder.h"
#include "utils/logger.h"
#include <cmath>

namespace cme_mock {

// SBEDecoder implementation
uint8_t SBEDecoder::decode_uint8() {
    if (!can_read(1)) return 0;
    return buffer_[offset_++];
}

uint16_t SBEDecoder::decode_uint16() {
    if (!can_read(2)) return 0;
    uint16_t value = buffer_[offset_] | (buffer_[offset_ + 1] << 8);
    offset_ += 2;
    return value;
}

uint32_t SBEDecoder::decode_uint32() {
    if (!can_read(4)) return 0;
    uint32_t value = buffer_[offset_] | 
                    (buffer_[offset_ + 1] << 8) |
                    (buffer_[offset_ + 2] << 16) |
                    (buffer_[offset_ + 3] << 24);
    offset_ += 4;
    return value;
}

uint64_t SBEDecoder::decode_uint64() {
    if (!can_read(8)) return 0;
    uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= static_cast<uint64_t>(buffer_[offset_ + i]) << (i * 8);
    }
    offset_ += 8;
    return value;
}

int32_t SBEDecoder::decode_int32() {
    return static_cast<int32_t>(decode_uint32());
}

int64_t SBEDecoder::decode_int64() {
    return static_cast<int64_t>(decode_uint64());
}

char SBEDecoder::decode_char() {
    if (!can_read(1)) return 0;
    return static_cast<char>(buffer_[offset_++]);
}

std::string SBEDecoder::decode_string(size_t length) {
    if (!can_read(length)) return "";
    std::string result(reinterpret_cast<const char*>(&buffer_[offset_]), length);
    offset_ += length;
    return result;
}

double SBEDecoder::decode_price(uint8_t decimals) {
    int64_t scaled_price = decode_int64();
    return static_cast<double>(scaled_price) / std::pow(10, decimals);
}

void SBEDecoder::skip(size_t bytes) {
    offset_ = std::min(offset_ + bytes, buffer_.size());
}

// MDPMessageDecoder implementation
bool MDPMessageDecoder::decode_packet_header(
    SBEDecoder& decoder,
    uint32_t& sequence_number,
    uint64_t& sending_time) {
    
    if (!decoder.can_read(12)) return false;
    
    sequence_number = decoder.decode_uint32();
    sending_time = decoder.decode_uint64();
    
    return true;
}

uint16_t MDPMessageDecoder::decode_message_header(
    SBEDecoder& decoder,
    uint16_t& block_length,
    uint16_t& schema_id,
    uint16_t& version) {
    
    if (!decoder.can_read(8)) return 0;
    
    block_length = decoder.decode_uint16();
    uint16_t template_id = decoder.decode_uint16();
    schema_id = decoder.decode_uint16();
    version = decoder.decode_uint16();
    
    return template_id;
}

bool MDPMessageDecoder::decode_snapshot_full_refresh(
    SBEDecoder& decoder,
    SnapshotFullRefresh& snapshot) {
    
    if (!decoder.can_read(28)) return false;
    
    // Decode message body
    snapshot.security_id = decoder.decode_uint32();
    snapshot.last_msg_seq_num_processed = decoder.decode_uint32();
    snapshot.rpt_seq = decoder.decode_uint32();
    snapshot.tot_num_reports = decoder.decode_uint32();
    snapshot.security_trading_status = decoder.decode_uint8();
    decoder.skip(7); // padding
    snapshot.transact_time = decoder.decode_uint64();
    
    // Decode group header
    if (!decoder.can_read(4)) return false;
    uint16_t num_entries = decoder.decode_uint16();
    uint16_t entry_length = decoder.decode_uint16();
    
    // Decode entries
    snapshot.entries.clear();
    snapshot.entries.reserve(num_entries);
    
    for (uint16_t i = 0; i < num_entries; ++i) {
        MDSnapshotEntry entry;
        if (!decode_snapshot_entry(decoder, entry)) {
            return false;
        }
        snapshot.entries.push_back(entry);
    }
    
    return true;
}

bool MDPMessageDecoder::decode_snapshot_entry(
    SBEDecoder& decoder,
    MDSnapshotEntry& entry) {
    
    if (!decoder.can_read(32)) return false;
    
    entry.entry_type = static_cast<MDEntryType>(decoder.decode_char());
    decoder.skip(7); // padding
    entry.price = decoder.decode_int64();
    entry.quantity = decoder.decode_int32();
    entry.number_of_orders = decoder.decode_uint32();
    entry.price_level = decoder.decode_uint8();
    decoder.skip(7); // padding
    
    return true;
}

bool MDPMessageDecoder::decode_incremental_refresh(
    SBEDecoder& decoder,
    IncrementalRefresh& incremental) {
    
    if (!decoder.can_read(8)) return false;
    
    incremental.transact_time = decoder.decode_uint64();
    
    // Decode price level group
    if (!decoder.can_read(4)) return false;
    uint16_t num_levels = decoder.decode_uint16();
    uint16_t level_length = decoder.decode_uint16();
    
    incremental.price_levels.clear();
    incremental.price_levels.reserve(num_levels);
    
    for (uint16_t i = 0; i < num_levels; ++i) {
        MDPriceLevel level;
        if (!decode_price_level(decoder, level)) {
            return false;
        }
        incremental.price_levels.push_back(level);
    }
    
    // Decode trade group
    if (!decoder.can_read(4)) return false;
    uint16_t num_trades = decoder.decode_uint16();
    uint16_t trade_length = decoder.decode_uint16();
    
    incremental.trades.clear();
    incremental.trades.reserve(num_trades);
    
    for (uint16_t i = 0; i < num_trades; ++i) {
        MDTrade trade;
        if (!decode_trade(decoder, trade)) {
            return false;
        }
        incremental.trades.push_back(trade);
    }
    
    return true;
}

bool MDPMessageDecoder::decode_price_level(
    SBEDecoder& decoder,
    MDPriceLevel& level) {
    
    if (!decoder.can_read(29)) return false;
    
    level.update_action = static_cast<MDUpdateAction>(decoder.decode_uint8());
    level.entry_type = static_cast<MDEntryType>(decoder.decode_char());
    decoder.skip(2); // padding
    level.security_id = decoder.decode_uint32();
    level.rpt_seq = decoder.decode_uint32();
    level.price = decoder.decode_int64();
    level.quantity = decoder.decode_int32();
    level.number_of_orders = decoder.decode_uint32();
    level.price_level = decoder.decode_uint8();
    // No additional padding - 29 bytes total
    
    return true;
}

bool MDPMessageDecoder::decode_trade(
    SBEDecoder& decoder,
    MDTrade& trade) {
    
    if (!decoder.can_read(32)) return false;
    
    trade.security_id = decoder.decode_uint32();
    trade.rpt_seq = decoder.decode_uint32();
    trade.price = decoder.decode_int64();
    trade.quantity = decoder.decode_int32();
    trade.number_of_orders = decoder.decode_uint32();
    trade.aggressor_side = decoder.decode_char();
    decoder.skip(7); // padding
    
    return true;
}

bool MDPMessageDecoder::decode_heartbeat(
    SBEDecoder& decoder,
    uint64_t& timestamp) {
    
    if (!decoder.can_read(8)) return false;
    timestamp = decoder.decode_uint64();
    return true;
}

bool MDPMessageDecoder::decode_channel_reset(
    SBEDecoder& decoder,
    uint64_t& timestamp) {
    
    if (!decoder.can_read(8)) return false;
    timestamp = decoder.decode_uint64();
    return true;
}

} // namespace cme_mock