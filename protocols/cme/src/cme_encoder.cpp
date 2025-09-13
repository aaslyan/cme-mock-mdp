#include "../include/cme_encoder.h"
#include <chrono>
#include <cstring>

namespace cme_protocol {

// Example implementation showing proper SBE usage pattern
std::vector<uint8_t> CMEEncoder::encode_incremental_refresh_book(
    const IncrementalRefresh& refresh)
{
    // 1. Allocate buffer for SBE to encode into
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);

    // 2. Create and wrap the message header
    cme_sbe::MessageHeader header;
    header.wrap(reinterpret_cast<char*>(buffer.data()), 0, 0, buffer.size())
        .blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
        .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    // 3. Wrap the actual message for encoding at the correct offset
    cme_sbe::MDIncrementalRefreshBook46 sbe_msg;
    sbe_msg.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        header.encodedLength(), // Start after header
        buffer.size());

    // 4. Use SBE's fluent API to set all fields
    // SBE handles all the binary encoding internally
    sbe_msg.transactTime(refresh.transact_time);

    // Set match event indicator
    sbe_msg.matchEventIndicator().clear();

    // 5. Encode repeating groups using SBE's API
    auto& entries = sbe_msg.noMDEntriesCount(refresh.entries.size());
    for (const auto& entry : refresh.entries) {
        auto& group_entry = entries.next();
        group_entry.securityID(entry.security_id)
            .rptSeq(entry.rpt_seq)
            .mDEntrySize(static_cast<int32_t>(entry.quantity))
            .numberOfOrders(entry.number_of_orders)
            .mDPriceLevel(entry.price_level)
            .mDUpdateAction(static_cast<cme_sbe::MDUpdateAction::Value>(entry.update_action))
            .mDEntryType(static_cast<cme_sbe::MDEntryTypeBook::Value>(entry.entry_type));

        // Set price separately to avoid chaining issues
        group_entry.mDEntryPx().mantissa(to_sbe_price(entry.price));
    }

    // 6. Second repeating group (usually empty for book updates)
    sbe_msg.noOrderIDEntriesCount(0);

    // 7. Get the actual encoded length from SBE
    size_t encoded_length = header.encodedLength() + sbe_msg.encodedLength();

    // 8. Resize buffer to actual size and return
    buffer.resize(encoded_length);
    return buffer;
}

std::vector<uint8_t> CMEEncoder::encode_snapshot_full_refresh(
    const SnapshotFullRefresh& snapshot)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);

    // Wrap header - SBE encodes it
    cme_sbe::MessageHeader header;
    header.wrap(reinterpret_cast<char*>(buffer.data()), 0, 0, buffer.size())
        .blockLength(cme_sbe::SnapshotFullRefresh52::sbeBlockLength())
        .templateId(cme_sbe::SnapshotFullRefresh52::sbeTemplateId())
        .schemaId(cme_sbe::SnapshotFullRefresh52::sbeSchemaId())
        .version(cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());

    // Wrap message - SBE handles encoding
    cme_sbe::SnapshotFullRefresh52 sbe_msg;
    sbe_msg.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        header.encodedLength(),
        buffer.size());

    // Use SBE API to set fields - SBE does all binary encoding
    sbe_msg.securityID(snapshot.security_id)
        .lastMsgSeqNumProcessed(snapshot.rpt_seq)
        .totNumReports(1)
        .transactTime(snapshot.transact_time)
        .tradeDate(0); // Will be set if needed

    // Encode entries using SBE's repeating group API
    size_t total_entries = snapshot.bid_entries.size() + snapshot.ask_entries.size();
    auto& entries = sbe_msg.noMDEntriesCount(total_entries);

    // Add bid entries - SBE encodes each one
    for (const auto& bid : snapshot.bid_entries) {
        auto& group_entry = entries.next();
        group_entry.mDEntrySize(static_cast<int32_t>(bid.quantity))
            .numberOfOrders(bid.number_of_orders)
            .mDPriceLevel(bid.price_level)
            .mDEntryType(cme_sbe::MDEntryType::Bid);

        // Set price separately
        group_entry.mDEntryPx().mantissa(to_sbe_price(bid.price));
    }

    // Add ask entries - SBE encodes each one
    for (const auto& ask : snapshot.ask_entries) {
        auto& group_entry = entries.next();
        group_entry.mDEntrySize(static_cast<int32_t>(ask.quantity))
            .numberOfOrders(ask.number_of_orders)
            .mDPriceLevel(ask.price_level)
            .mDEntryType(cme_sbe::MDEntryType::Offer);

        // Set price separately
        group_entry.mDEntryPx().mantissa(to_sbe_price(ask.price));
    }

    // Get actual size from SBE
    size_t encoded_length = header.encodedLength() + sbe_msg.encodedLength();
    buffer.resize(encoded_length);
    return buffer;
}

std::vector<uint8_t> CMEEncoder::encode_security_definition_future(
    const SecurityDefinition& definition)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);

    // Let SBE encode the header
    cme_sbe::MessageHeader header;
    header.wrap(reinterpret_cast<char*>(buffer.data()), 0, 0, buffer.size())
        .blockLength(cme_sbe::MDInstrumentDefinitionFuture54::sbeBlockLength())
        .templateId(cme_sbe::MDInstrumentDefinitionFuture54::sbeTemplateId())
        .schemaId(cme_sbe::MDInstrumentDefinitionFuture54::sbeSchemaId())
        .version(cme_sbe::MDInstrumentDefinitionFuture54::sbeSchemaVersion());

    // Let SBE encode the message
    cme_sbe::MDInstrumentDefinitionFuture54 sbe_msg;
    sbe_msg.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        header.encodedLength(),
        buffer.size());

    // Use SBE's fluent API - it handles all encoding
    sbe_msg.securityID(definition.security_id)
        .securityUpdateAction(static_cast<cme_sbe::SecurityUpdateAction::Value>(definition.update_action));

    // Set prices separately to avoid chaining issues
    sbe_msg.minPriceIncrement().mantissa(to_sbe_price(definition.min_price_increment));
    sbe_msg.displayFactor().mantissa(static_cast<int64_t>(definition.display_factor));

    // Set string fields using SBE's API
    sbe_msg.putSymbol(definition.symbol.c_str());
    sbe_msg.putSecurityGroup(definition.security_group.c_str());
    sbe_msg.putAsset(definition.asset.c_str());
    sbe_msg.putCurrency(definition.currency.c_str());

    // Empty repeating groups
    sbe_msg.noEventsCount(0);
    sbe_msg.noMDFeedTypesCount(0);
    sbe_msg.noInstAttribCount(0);
    sbe_msg.noLotTypeRulesCount(0);

    size_t encoded_length = header.encodedLength() + sbe_msg.encodedLength();
    buffer.resize(encoded_length);
    return buffer;
}

// Packet header - this is NOT SBE, it's CME's packet format
std::vector<uint8_t> CMEEncoder::encode_packet_header(
    uint32_t sequence_number,
    uint64_t sending_time)
{
    std::vector<uint8_t> header(12);
    // Little-endian encoding for packet header
    std::memcpy(header.data(), &sequence_number, 4);
    std::memcpy(header.data() + 4, &sending_time, 8);
    return header;
}

// Message size field - also NOT SBE, it's CME's packet format
std::vector<uint8_t> CMEEncoder::encode_message_size(uint16_t message_size)
{
    std::vector<uint8_t> size_field(2);
    // Little-endian encoding
    std::memcpy(size_field.data(), &message_size, 2);
    return size_field;
}

// Create complete packet with SBE messages
std::vector<uint8_t> CMEEncoder::create_packet(
    uint32_t sequence_number,
    uint64_t sending_time,
    const std::vector<std::vector<uint8_t>>& sbe_messages)
{
    std::vector<uint8_t> packet;

    // Add packet header (not SBE)
    auto header = encode_packet_header(sequence_number, sending_time);
    packet.insert(packet.end(), header.begin(), header.end());

    // Add each SBE message with its size field
    for (const auto& sbe_message : sbe_messages) {
        // Add message size field (not SBE)
        auto size_field = encode_message_size(sbe_message.size());
        packet.insert(packet.end(), size_field.begin(), size_field.end());

        // Add the SBE-encoded message
        packet.insert(packet.end(), sbe_message.begin(), sbe_message.end());
    }

    return packet;
}

std::vector<uint8_t> CMEEncoder::encode_security_status(
    uint32_t security_id,
    uint8_t trading_status,
    uint64_t transact_time)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);

    cme_sbe::MessageHeader header;
    header.wrap(reinterpret_cast<char*>(buffer.data()), 0, 0, buffer.size())
        .blockLength(cme_sbe::SecurityStatus30::sbeBlockLength())
        .templateId(cme_sbe::SecurityStatus30::sbeTemplateId())
        .schemaId(cme_sbe::SecurityStatus30::sbeSchemaId())
        .version(cme_sbe::SecurityStatus30::sbeSchemaVersion());

    cme_sbe::SecurityStatus30 sbe_msg;
    sbe_msg.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        header.encodedLength(),
        buffer.size());

    sbe_msg.securityID(security_id)
        .transactTime(transact_time);
    // tradingStatus field would be set here if available in SBE schema

    size_t encoded_length = header.encodedLength() + sbe_msg.encodedLength();
    buffer.resize(encoded_length);
    return buffer;
}

std::vector<uint8_t> CMEEncoder::encode_session_statistics(
    const MDStatistics& stats)
{
    std::vector<uint8_t> buffer(MAX_MESSAGE_SIZE);

    cme_sbe::MessageHeader header;
    header.wrap(reinterpret_cast<char*>(buffer.data()), 0, 0, buffer.size())
        .blockLength(cme_sbe::MDIncrementalRefreshSessionStatistics51::sbeBlockLength())
        .templateId(cme_sbe::MDIncrementalRefreshSessionStatistics51::sbeTemplateId())
        .schemaId(cme_sbe::MDIncrementalRefreshSessionStatistics51::sbeSchemaId())
        .version(cme_sbe::MDIncrementalRefreshSessionStatistics51::sbeSchemaVersion());

    cme_sbe::MDIncrementalRefreshSessionStatistics51 sbe_msg;
    sbe_msg.wrapForEncode(
        reinterpret_cast<char*>(buffer.data()),
        header.encodedLength(),
        buffer.size());

    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                             .count();
    sbe_msg.transactTime(timestamp);

    // Add statistics entries (stub implementation)
    auto& entries = sbe_msg.noMDEntriesCount(1);
    entries.next()
        .securityID(stats.security_id);

    size_t encoded_length = header.encodedLength() + sbe_msg.encodedLength();
    buffer.resize(encoded_length);
    return buffer;
}

int64_t CMEEncoder::to_sbe_price(double price, int decimals)
{
    double multiplier = 1.0;
    for (int i = 0; i < decimals; ++i) {
        multiplier *= 10.0;
    }
    return static_cast<int64_t>(price * multiplier);
}

} // namespace cme_protocol