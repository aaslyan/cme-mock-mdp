#include "../include/reuters_encoder.h"
#include "../include/lseg_sbe/Establish.h"
#include "../include/lseg_sbe/Heartbeat.h"
#include "../include/lseg_sbe/MarketDataIncrementalRefresh.h"
#include "../include/lseg_sbe/MarketDataRequestRejection.h"
#include "../include/lseg_sbe/MarketDataSnapshotFullRefresh.h"
#include "../include/lseg_sbe/MessageHeader.h"
#include "../include/lseg_sbe/Negotiate.h"
#include "../include/lseg_sbe/SecurityDefinition.h"
#include "../include/lseg_sbe/SessionReject.h"
#include "../include/lseg_sbe/Terminate.h"
#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <cstring>

namespace reuters_protocol {

std::vector<uint8_t> ReutersEncoder::encode_negotiate_response(
    const std::string& session_id,
    FlowType flow_type,
    const std::string& username,
    const std::string& password)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);

    // Create SOFH header space
    size_t message_offset = SOFHeader::size();

    // Create message compatible with sample client expectations
    // Use manual binary packing to match client's schema v22 format

    // SBE Message Header (12 bytes) - compatible with sample client
    uint16_t root_block_length = 56; // Expected by client
    uint16_t template_id = 500; // Negotiate template
    uint16_t schema_id = 101; // LSEG schema ID
    uint16_t schema_version = 22; // Client expects version 22!
    uint16_t num_groups = 0;
    uint16_t num_var_data = 0;

    size_t header_size = 12;
    size_t body_size = 56; // Negotiate message body size expected by client
    size_t total_sbe_size = header_size + body_size;

    // Pack SBE header manually (big-endian as per LSEG spec)
    size_t pos = message_offset;
    *reinterpret_cast<uint16_t*>(buffer.data() + pos) = htons(root_block_length);
    pos += 2;
    *reinterpret_cast<uint16_t*>(buffer.data() + pos) = htons(template_id);
    pos += 2;
    *reinterpret_cast<uint16_t*>(buffer.data() + pos) = htons(schema_id);
    pos += 2;
    *reinterpret_cast<uint16_t*>(buffer.data() + pos) = htons(schema_version);
    pos += 2;
    *reinterpret_cast<uint16_t*>(buffer.data() + pos) = htons(num_groups);
    pos += 2;
    *reinterpret_cast<uint16_t*>(buffer.data() + pos) = htons(num_var_data);
    pos += 2;

    // Pack Negotiate message body (56 bytes) - matching client layout
    *reinterpret_cast<uint8_t*>(buffer.data() + pos) = 'O';
    pos += 1; // NegotiationResponse

    // SessionID (20 bytes, null-padded)
    memset(buffer.data() + pos, 0, 20);
    strncpy(reinterpret_cast<char*>(buffer.data() + pos), session_id.c_str(), std::min(session_id.length(), size_t(20)));
    pos += 20;

    // Timestamp (8 bytes, big-endian)
    uint64_t timestamp = get_current_timestamp_ns();
    *reinterpret_cast<uint64_t*>(buffer.data() + pos) = htonl(timestamp >> 32) | (static_cast<uint64_t>(htonl(timestamp & 0xFFFFFFFF)) << 32);
    pos += 8;

    // Flow type (1 byte)
    *reinterpret_cast<uint8_t*>(buffer.data() + pos) = static_cast<uint8_t>(flow_type);
    pos += 1;

    // Credentials (11 + 15 = 26 bytes, null-padded)
    memset(buffer.data() + pos, 0, 26);
    if (!username.empty()) {
        strncpy(reinterpret_cast<char*>(buffer.data() + pos), username.c_str(), std::min(username.length(), size_t(11)));
    }
    pos += 11;
    if (!password.empty()) {
        strncpy(reinterpret_cast<char*>(buffer.data() + pos), password.c_str(), std::min(password.length(), size_t(15)));
    }
    pos += 15;

    // Create and encode SOFH header
    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + total_sbe_size);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    // Resize buffer to actual size
    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_negotiate_reject(
    const std::string& session_id,
    SessionRejectCode reject_code,
    const std::string& reason)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::SessionReject reject;
    reject.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    reject.messageType(lseg_sbe::MessageTypeEnum::Value::NegotiationReject)
        .putSessionID(session_id)
        .rejectCode(static_cast<lseg_sbe::SessionRejectCodeEnum::Value>(reject_code));
    reject.timestamp().time(get_current_timestamp_ns());

    // Set variable-length reason field
    reject.putReason(reason.c_str(), reason.length());

    size_t sbe_encoded_length = reject.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_establish_ack(
    const std::string& session_id,
    uint32_t keepalive_interval)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::Establish establish;
    establish.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    establish.messageType(lseg_sbe::MessageTypeEnum::Value::EstablishmentAck)
        .putSessionID(session_id)
        .keepAliveInterval(keepalive_interval);
    establish.timestamp().time(get_current_timestamp_ns());

    size_t sbe_encoded_length = establish.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_establish_reject(
    const std::string& session_id,
    SessionRejectCode reject_code,
    const std::string& reason)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::SessionReject reject;
    reject.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    reject.messageType(lseg_sbe::MessageTypeEnum::Value::EstablishmentReject)
        .putSessionID(session_id)
        .rejectCode(static_cast<lseg_sbe::SessionRejectCodeEnum::Value>(reject_code));
    reject.timestamp().time(get_current_timestamp_ns());

    reject.putReason(reason.c_str(), reason.length());

    size_t sbe_encoded_length = reject.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_terminate(
    const std::string& session_id,
    TerminationCode termination_code,
    const std::string& reason)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::Terminate terminate;
    terminate.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    terminate.messageType(lseg_sbe::MessageTypeEnum::Value::Terminate)
        .putSessionID(session_id)
        .terminationCode(static_cast<lseg_sbe::TerminationCodeEnum::Value>(termination_code));

    terminate.putReason(reason.c_str(), reason.length());

    size_t sbe_encoded_length = terminate.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_heartbeat()
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::Heartbeat heartbeat;
    heartbeat.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    heartbeat.messageType(lseg_sbe::MessageTypeEnum::Value::UnsequencedHeartbeat);

    size_t sbe_encoded_length = heartbeat.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_security_definition(
    const market_core::Instrument& instrument)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::SecurityDefinition secDef;
    secDef.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    // Map our core Instrument to LSEG SecurityDefinition
    secDef.putApplID("FXMD01") // Application ID
        .securityStatus(lseg_sbe::SecurityStatusEnum::Value::Active)
        .putSymbol(instrument.primary_symbol);

    // Set FX-specific fields if it's an FX instrument
    if (instrument.get_type() == market_core::InstrumentType::FX_SPOT) {
        secDef.putSecurityType("FOR") // FX Forward/Spot
            .putCurrency1("USD") // Extract from instrument
            .putCurrency2("EUR") // Extract from instrument
            .exDestination(lseg_sbe::ExDestinationEnum::Value::MTF)
            .marketSegmentID(lseg_sbe::MarketSegmentIDEnum::Value::Regular);
    }

    size_t sbe_encoded_length = secDef.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_market_data_snapshot(
    const market_core::SnapshotEvent& snapshot)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::MarketDataSnapshotFullRefresh mdSnapshot;
    mdSnapshot.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    // Set snapshot header fields
    mdSnapshot.putApplID("FXMD01")
        .putSecurityID(std::to_string(snapshot.instrument_id))
        .putSecurityType("FOR");

    // Create MD entries for bid/ask levels
    auto& entries = mdSnapshot.mDEntriesCount(snapshot.bid_levels.size() + snapshot.ask_levels.size());

    // Add bid levels
    for (size_t i = 0; i < snapshot.bid_levels.size(); ++i) {
        auto& entry = entries.next();
        entry.mDEntryType(lseg_sbe::EntryTypeEnum::Value::Bid)
            .mDPriceLevel(static_cast<uint32_t>(i + 1))
            .numberOfOrders(snapshot.bid_levels[i].order_count);
        entry.mDEntryPx().mantissa(to_sbe_decimal(snapshot.bid_levels[i].price));
        entry.mDEntrySize().mantissa(to_sbe_quantity(snapshot.bid_levels[i].quantity));
    }

    // Add ask levels
    for (size_t i = 0; i < snapshot.ask_levels.size(); ++i) {
        auto& entry = entries.next();
        entry.mDEntryType(lseg_sbe::EntryTypeEnum::Value::Offer)
            .mDPriceLevel(static_cast<uint32_t>(i + 1))
            .numberOfOrders(snapshot.ask_levels[i].order_count);
        entry.mDEntryPx().mantissa(to_sbe_decimal(snapshot.ask_levels[i].price));
        entry.mDEntrySize().mantissa(to_sbe_quantity(snapshot.ask_levels[i].quantity));
    }

    size_t sbe_encoded_length = mdSnapshot.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_market_data_incremental(
    const market_core::QuoteEvent& quote)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::MarketDataIncrementalRefresh mdIncremental;
    mdIncremental.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    // Set incremental header
    mdIncremental.putApplID("FXMD01");

    // Create single MD entry for the quote update
    auto& entries = mdIncremental.mDIncGrpCount(1);
    auto& entry = entries.next();
    entry.mDUpdateAction(static_cast<lseg_sbe::UpdateActionEnum::Value>(quote.action))
        .mDEntryType(quote.side == market_core::Side::BID ? lseg_sbe::EntryTypeEnum::Value::Bid : lseg_sbe::EntryTypeEnum::Value::Offer)
        .putSecurityID(std::to_string(quote.instrument_id))
        .mDPriceLevel(quote.price_level.value_or(1))
        .numberOfOrders(quote.order_count);
    entry.mDEntryPx().mantissa(to_sbe_decimal(quote.price));
    entry.mDEntrySize().mantissa(to_sbe_quantity(quote.quantity));

    size_t sbe_encoded_length = mdIncremental.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_market_data_incremental(
    const market_core::TradeEvent& trade)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::MarketDataIncrementalRefresh mdIncremental;
    mdIncremental.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    mdIncremental.putApplID("FXMD01");

    // Create trade entry
    auto& entries = mdIncremental.mDIncGrpCount(1);
    auto& entry = entries.next();
    entry.mDUpdateAction(lseg_sbe::UpdateActionEnum::Value::New)
        .mDEntryType(lseg_sbe::EntryTypeEnum::Value::Trade)
        .putSecurityID(std::to_string(trade.instrument_id))
        .aggressorSide(trade.aggressor_side == market_core::Side::BID ? lseg_sbe::AggressorSideEnum::Value::Buyer : lseg_sbe::AggressorSideEnum::Value::Seller);
    entry.mDEntryPx().mantissa(to_sbe_decimal(trade.price));
    entry.mDEntrySize().mantissa(to_sbe_quantity(trade.quantity));

    size_t sbe_encoded_length = mdIncremental.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

std::vector<uint8_t> ReutersEncoder::encode_market_data_request_rejection(
    const std::string& md_req_id,
    const std::string& rejection_reason)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);
    size_t message_offset = SOFHeader::size();

    lseg_sbe::MarketDataRequestRejection reject;
    reject.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        message_offset,
        buffer.size() - message_offset);

    reject.putMDReqID(md_req_id)
        .putMDReqRejReason(rejection_reason.c_str(), rejection_reason.length());

    size_t sbe_encoded_length = reject.encodedLength();

    SOFHeader sofh;
    sofh.message_length = static_cast<uint32_t>(SOFHeader::size() + sbe_encoded_length);
    sofh.encoding_type = BIG_ENDIAN_ENCODING;
    sofh.decryption_id = 0;
    sofh.pack(buffer.data());

    buffer.resize(sofh.message_length);
    return buffer;
}

// Helper methods
uint64_t ReutersEncoder::get_current_timestamp_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count();
}

int64_t ReutersEncoder::to_sbe_decimal(double price)
{
    // LSEG uses decimal encoding with -8 exponent (8 decimal places)
    return static_cast<int64_t>(price * 100000000.0);
}

int64_t ReutersEncoder::to_sbe_quantity(uint64_t quantity)
{
    // LSEG uses quantity encoding with -2 exponent (2 decimal places)
    return static_cast<int64_t>(quantity * 100);
}

} // namespace reuters_protocol