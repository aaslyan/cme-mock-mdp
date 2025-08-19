#include "messages/sbe_encoder.h"
#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "cme_sbe/SnapshotFullRefresh52.h"
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

    // CME MDP 3.0 Binary Packet Header (12 bytes total)
    encoder.encode_uint32(sequence_number); // Offset 0-3
    encoder.encode_uint64(sending_time); // Offset 4-11

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

    // Force Schema Version 13 for all instruments to match gateway client expectations
    uint16_t template_id = TEMPLATE_FX_SNAPSHOT_FULL_REFRESH; // Always use FX template (52)
    uint16_t schema_id = MDP_SCHEMA_ID_FX; // Always use FX schema (1)
    uint16_t version = MDP_VERSION_FX; // Always use FX version (13)

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
    std::vector<char> buffer(1024);

    // Step 1: Encode CME-specific SBE header (not standard SBE MessageHeader)
    size_t offset = 0;

    // Binary Size - will be filled in later after we know total size
    size_t binary_size_offset = offset;
    buffer[offset++] = 0; // placeholder
    buffer[offset++] = 0; // placeholder

    // Length (4 bytes) - SBE header size = 10 bytes
    uint32_t sbe_header_length = 10;
    buffer[offset++] = sbe_header_length & 0xFF;
    buffer[offset++] = (sbe_header_length >> 8) & 0xFF;
    buffer[offset++] = (sbe_header_length >> 16) & 0xFF;
    buffer[offset++] = (sbe_header_length >> 24) & 0xFF;

    // TemplateID (2 bytes)
    uint16_t template_id = cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId();
    buffer[offset++] = template_id & 0xFF;
    buffer[offset++] = (template_id >> 8) & 0xFF;

    // SchemaID (2 bytes)
    uint16_t schema_id = cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId();
    buffer[offset++] = schema_id & 0xFF;
    buffer[offset++] = (schema_id >> 8) & 0xFF;

    // Version (2 bytes)
    uint16_t version = cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion();
    buffer[offset++] = version & 0xFF;
    buffer[offset++] = (version >> 8) & 0xFF;

    size_t header_size = offset; // Should be 12 bytes (2 + 4 + 2 + 2 + 2)

    // Step 2: Encode Message Body using CME SBE constructor
    cme_sbe::MDIncrementalRefreshBook46 message(
        buffer.data(),
        header_size,
        buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    // Set message fields
    message.transactTime(incremental.transact_time);

    // Add price level entries
    auto& entries = message.noMDEntriesCount(static_cast<std::uint8_t>(incremental.price_levels.size()));
    for (size_t i = 0; i < incremental.price_levels.size(); ++i) {
        auto& entry = entries.next();
        const auto& level = incremental.price_levels[i];

        entry.mDUpdateAction(static_cast<cme_sbe::MDUpdateAction::Value>(level.update_action));
        entry.mDEntryType(static_cast<cme_sbe::MDEntryTypeBook::Value>(level.entry_type));
        entry.securityID(level.security_id);
        entry.rptSeq(level.rpt_seq);
        entry.mDEntryPx().mantissa(level.price);
        entry.mDEntrySize(level.quantity);
        entry.numberOfOrders(level.number_of_orders);
        entry.mDPriceLevel(level.price_level);
    }

    // Add empty Order ID entries group (required by CME specification)
    message.noOrderIDEntriesCount(0);

    // Calculate total message size and fill in Binary Size field
    size_t body_length = message.encodedLength() - header_size;
    size_t total_length = header_size + body_length;

    // Fill in Binary Size (total message size including header)
    uint16_t binary_size = static_cast<uint16_t>(total_length);
    buffer[binary_size_offset] = binary_size & 0xFF;
    buffer[binary_size_offset + 1] = (binary_size >> 8) & 0xFF;

    // Convert to uint8_t vector
    std::vector<uint8_t> result(buffer.begin(), buffer.begin() + total_length);
    return result;
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