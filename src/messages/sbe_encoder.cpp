#include "messages/sbe_encoder.h"
#include <algorithm>
#include <cmath>

namespace cme_mock {

// SBEEncoder implementation
void SBEEncoder::encode_uint8(uint8_t value)
{
    ensure_capacity(1);
    buffer_[offset_++] = value;
}

void SBEEncoder::encode_uint16(uint16_t value)
{
    ensure_capacity(2);
    // Little-endian encoding
    buffer_[offset_++] = value & 0xFF;
    buffer_[offset_++] = (value >> 8) & 0xFF;
}

void SBEEncoder::encode_uint32(uint32_t value)
{
    ensure_capacity(4);
    buffer_[offset_++] = value & 0xFF;
    buffer_[offset_++] = (value >> 8) & 0xFF;
    buffer_[offset_++] = (value >> 16) & 0xFF;
    buffer_[offset_++] = (value >> 24) & 0xFF;
}

void SBEEncoder::encode_uint64(uint64_t value)
{
    ensure_capacity(8);
    for (int i = 0; i < 8; ++i) {
        buffer_[offset_++] = (value >> (i * 8)) & 0xFF;
    }
}

void SBEEncoder::encode_int32(int32_t value)
{
    encode_uint32(static_cast<uint32_t>(value));
}

void SBEEncoder::encode_int64(int64_t value)
{
    encode_uint64(static_cast<uint64_t>(value));
}

void SBEEncoder::encode_char(char value)
{
    ensure_capacity(1);
    buffer_[offset_++] = static_cast<uint8_t>(value);
}

void SBEEncoder::encode_string(const char* str, size_t length)
{
    ensure_capacity(length);
    std::memcpy(&buffer_[offset_], str, length);
    offset_ += length;
}

void SBEEncoder::encode_price(double price, uint8_t decimals)
{
    // Convert price to fixed-point representation
    int64_t scaled_price = static_cast<int64_t>(price * std::pow(10, decimals));
    encode_int64(scaled_price);
}

void SBEEncoder::encode_quantity(int32_t quantity)
{
    encode_int32(quantity);
}

void SBEEncoder::ensure_capacity(size_t required)
{
    if (offset_ + required > buffer_.size()) {
        buffer_.resize(offset_ + required + 1024);
    }
}

// MDPMessageEncoder implementation
std::vector<uint8_t> MDPMessageEncoder::encode_packet_header(
    uint32_t sequence_number,
    uint64_t sending_time)
{

    SBEEncoder encoder;

    // Packet header (simplified)
    encoder.encode_uint32(sequence_number);
    encoder.encode_uint64(sending_time);

    return encoder.get_buffer();
}

void MDPMessageEncoder::encode_message_header(
    SBEEncoder& encoder,
    uint16_t template_id,
    uint16_t schema_id,
    uint16_t version,
    uint16_t block_length)
{

    // SBE message header
    encoder.encode_uint16(block_length);
    encoder.encode_uint16(template_id);
    encoder.encode_uint16(schema_id);
    encoder.encode_uint16(version);
}

std::vector<uint8_t> MDPMessageEncoder::encode_snapshot_full_refresh(
    const SnapshotFullRefresh& snapshot)
{

    SBEEncoder encoder;

    // Determine if this is an FX instrument (Channel 330, security IDs 31-40)
    bool is_fx_instrument = (snapshot.security_id >= 31 && snapshot.security_id <= 40);

    // Use appropriate schema version and template ID
    uint16_t template_id = is_fx_instrument ? TEMPLATE_FX_SNAPSHOT_FULL_REFRESH : TEMPLATE_SNAPSHOT_FULL_REFRESH;
    uint16_t schema_id = is_fx_instrument ? MDP_SCHEMA_ID_FX : MDP_SCHEMA_ID;
    uint16_t version = is_fx_instrument ? MDP_VERSION_FX : MDP_VERSION;

    // Message header
    encode_message_header(encoder, template_id, schema_id, version, BLOCK_LENGTH_SNAPSHOT);

    // Message body
    encoder.encode_uint32(snapshot.security_id);
    encoder.encode_uint32(snapshot.last_msg_seq_num_processed);
    encoder.encode_uint32(snapshot.rpt_seq);
    encoder.encode_uint32(snapshot.tot_num_reports);
    encoder.encode_uint8(snapshot.security_trading_status);
    encoder.encode_uint64(snapshot.transact_time);

    // Number of entries (group header)
    encoder.encode_uint16(static_cast<uint16_t>(snapshot.entries.size()));
    encoder.encode_uint16(32); // Entry block length

    // Encode each entry
    for (const auto& entry : snapshot.entries) {
        encode_snapshot_entry(encoder, entry);
    }

    return encoder.get_buffer();
}

void MDPMessageEncoder::encode_snapshot_entry(
    SBEEncoder& encoder,
    const MDSnapshotEntry& entry)
{

    encoder.encode_char(static_cast<char>(entry.entry_type));
    encoder.encode_int64(entry.price);
    encoder.encode_int32(entry.quantity);
    encoder.encode_uint32(entry.number_of_orders);
    encoder.encode_uint8(entry.price_level);

    // Padding for alignment
    encoder.encode_uint8(0);
    encoder.encode_uint16(0);
}

std::vector<uint8_t> MDPMessageEncoder::encode_incremental_refresh(
    const IncrementalRefresh& incremental)
{

    SBEEncoder encoder;

    // Determine if this contains FX instruments (Channel 330, security IDs 31-40)
    bool has_fx_instrument = false;
    for (const auto& level : incremental.price_levels) {
        if (level.security_id >= 31 && level.security_id <= 40) {
            has_fx_instrument = true;
            break;
        }
    }
    if (!has_fx_instrument) {
        for (const auto& trade : incremental.trades) {
            if (trade.security_id >= 31 && trade.security_id <= 40) {
                has_fx_instrument = true;
                break;
            }
        }
    }

    // Use appropriate schema version and template ID
    uint16_t template_id = has_fx_instrument ? TEMPLATE_FX_INCREMENTAL_REFRESH_BOOK : TEMPLATE_INCREMENTAL_REFRESH_BOOK;
    uint16_t schema_id = has_fx_instrument ? MDP_SCHEMA_ID_FX : MDP_SCHEMA_ID;
    uint16_t version = has_fx_instrument ? MDP_VERSION_FX : MDP_VERSION;

    // Message header
    encode_message_header(encoder, template_id, schema_id, version, BLOCK_LENGTH_INCREMENTAL);

    // Message body
    encoder.encode_uint64(incremental.transact_time);

    // Price level entries
    encoder.encode_uint16(static_cast<uint16_t>(incremental.price_levels.size()));
    encoder.encode_uint16(29); // Entry block length (actual size)

    for (const auto& level : incremental.price_levels) {
        encode_price_level(encoder, level);
    }

    // Trade entries
    encoder.encode_uint16(static_cast<uint16_t>(incremental.trades.size()));
    encoder.encode_uint16(32); // Trade block length

    for (const auto& trade : incremental.trades) {
        encode_trade(encoder, trade);
    }

    return encoder.get_buffer();
}

void MDPMessageEncoder::encode_price_level(
    SBEEncoder& encoder,
    const MDPriceLevel& level)
{

    encoder.encode_uint8(static_cast<uint8_t>(level.update_action));
    encoder.encode_char(static_cast<char>(level.entry_type));
    encoder.encode_uint16(0); // padding for alignment
    encoder.encode_uint32(level.security_id);
    encoder.encode_uint32(level.rpt_seq);
    encoder.encode_int64(level.price);
    encoder.encode_int32(level.quantity);
    encoder.encode_uint32(level.number_of_orders);
    encoder.encode_uint8(level.price_level);

    // No additional padding - 29 bytes total
}

void MDPMessageEncoder::encode_trade(
    SBEEncoder& encoder,
    const MDTrade& trade)
{

    encoder.encode_uint32(trade.security_id);
    encoder.encode_uint32(trade.rpt_seq);
    encoder.encode_int64(trade.price);
    encoder.encode_int32(trade.quantity);
    encoder.encode_uint32(trade.number_of_orders);
    encoder.encode_char(trade.aggressor_side);

    // Padding
    encoder.encode_uint8(0);
    encoder.encode_uint16(0);
}

std::vector<uint8_t> MDPMessageEncoder::encode_channel_reset(
    uint64_t transaction_time)
{

    SBEEncoder encoder;

    encode_message_header(encoder, TEMPLATE_CHANNEL_RESET,
        MDP_SCHEMA_ID, MDP_VERSION, 8);

    encoder.encode_uint64(transaction_time);

    return encoder.get_buffer();
}

std::vector<uint8_t> MDPMessageEncoder::encode_heartbeat(
    uint64_t timestamp)
{

    SBEEncoder encoder;

    encode_message_header(encoder, TEMPLATE_ADMIN_HEARTBEAT,
        MDP_SCHEMA_ID, MDP_VERSION, 8);

    encoder.encode_uint64(timestamp);

    return encoder.get_buffer();
}

} // namespace cme_mock