#include "messages/cme_sbe_encoder.h"
#include "utils/logger.h"
#include <chrono>

namespace cme_mock {

bool CMESBEEncoder::is_fx_instrument(uint32_t security_id)
{
    // FX instruments are security IDs 31-40 (Channel 330)
    return security_id >= 31 && security_id <= 40;
}

std::vector<uint8_t> CMESBEEncoder::encode_packet_header(
    uint32_t sequence_number,
    uint64_t sending_time)
{
    std::vector<uint8_t> buffer(12); // 4 + 8 bytes: sequence + timestamp only

    // Encode sequence number (little-endian)
    buffer[0] = sequence_number & 0xFF;
    buffer[1] = (sequence_number >> 8) & 0xFF;
    buffer[2] = (sequence_number >> 16) & 0xFF;
    buffer[3] = (sequence_number >> 24) & 0xFF;

    // Encode sending time (little-endian)
    for (int i = 0; i < 8; ++i) {
        buffer[4 + i] = (sending_time >> (i * 8)) & 0xFF;
    }

    return buffer;
}

std::vector<uint8_t> CMESBEEncoder::encode_message_size(uint16_t message_size)
{
    std::vector<uint8_t> buffer(2);
    // Encode message size (little-endian)
    buffer[0] = message_size & 0xFF;
    buffer[1] = (message_size >> 8) & 0xFF;
    return buffer;
}

std::vector<uint8_t> CMESBEEncoder::encode_multi_message_packet(
    uint32_t sequence_number,
    uint64_t sending_time,
    const std::vector<std::vector<uint8_t>>& messages)
{
    std::vector<uint8_t> packet;

    // Add packet header (12 bytes)
    auto header = encode_packet_header(sequence_number, sending_time);
    packet.insert(packet.end(), header.begin(), header.end());

    // Add each message with its size field
    for (const auto& message : messages) {
        // Add message size (2 bytes)
        auto msg_size = encode_message_size(static_cast<uint16_t>(message.size()));
        packet.insert(packet.end(), msg_size.begin(), msg_size.end());

        // Add the SBE message
        packet.insert(packet.end(), message.begin(), message.end());
    }

    return packet;
}

std::vector<uint8_t> CMESBEEncoder::encode_snapshot_full_refresh(
    const SnapshotFullRefresh& snapshot)
{
    // Calculate buffer size - need space for header + message + groups
    // Header: ~8 bytes, Message: ~50 bytes, Group header: 8 bytes, Entries: ~40 bytes each
    size_t estimated_entries = std::min(snapshot.entries.size(), size_t(5));
    size_t buffer_size = 8 + 50 + 8 + (estimated_entries * 40) + 512; // Add padding
    std::vector<char> buffer(buffer_size);

    try {
        // Step 1: Wrap and encode the message header
        cme_sbe::MessageHeader header;
        header.wrap(buffer.data(), 0, 0, buffer.size())
            .blockLength(cme_sbe::SnapshotFullRefresh52::sbeBlockLength())
            .templateId(cme_sbe::SnapshotFullRefresh52::sbeTemplateId())
            .schemaId(cme_sbe::SnapshotFullRefresh52::sbeSchemaId())
            .version(cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());

        // Step 2: Wrap for encode and populate the message body
        size_t header_size = header.encodedLength();
        cme_sbe::SnapshotFullRefresh52 sbe_msg;
        sbe_msg.wrapForEncode(buffer.data(), header_size, buffer.size());

        // Set basic fields using method chaining
        sbe_msg.securityID(snapshot.security_id)
            .lastMsgSeqNumProcessed(snapshot.last_msg_seq_num_processed)
            .rptSeq(snapshot.rpt_seq)
            .totNumReports(snapshot.tot_num_reports)
            .transactTime(snapshot.transact_time);

        // Add entries using the group API
        size_t num_entries = std::min(snapshot.entries.size(), size_t(5));
        auto& entries_group = sbe_msg.noMDEntriesCount(static_cast<uint16_t>(num_entries));

        for (size_t i = 0; i < num_entries; ++i) {
            const auto& entry = snapshot.entries[i];

            // Use next() to advance to the next entry and chain field setters
            entries_group.next()
                .mDPriceLevel(entry.price_level);

            // Set price using the PRICE9 composite
            entries_group.mDEntryPx().mantissa(entry.price);
        }

        // Get the total encoded length (header + message)
        size_t total_length = header_size + sbe_msg.encodedLength();
        buffer.resize(total_length);

        // Convert char buffer to uint8_t buffer
        return std::vector<uint8_t>(buffer.begin(), buffer.end());

    } catch (const std::exception& e) {
        LOG_ERROR("SBE snapshot encoding error: " + std::string(e.what()));
        // Fall back to minimal message
        std::vector<uint8_t> minimal_buffer(64, 0);
        return minimal_buffer;
    }
}

std::vector<uint8_t> CMESBEEncoder::encode_incremental_refresh(
    const IncrementalRefresh& incremental)
{
    // Calculate buffer size - need space for header + message + groups
    // Header: ~8 bytes, Message: ~30 bytes, NoMDEntries: 8 bytes header + entries
    size_t estimated_entries = std::min(incremental.price_levels.size(), size_t(5));
    size_t buffer_size = 8 + 30 + 8 + (estimated_entries * 50) + 512; // Add padding
    std::vector<char> buffer(buffer_size);

    try {
        // Step 1: Wrap and encode the message header
        cme_sbe::MessageHeader header;
        header.wrap(buffer.data(), 0, 0, buffer.size())
            .blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
            .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
            .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
            .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

        // Step 2: Wrap for encode and populate the message body
        size_t header_size = header.encodedLength();
        cme_sbe::MDIncrementalRefreshBook46 sbe_msg;
        sbe_msg.wrapForEncode(buffer.data(), header_size, buffer.size());

        // Set basic fields using method chaining
        sbe_msg.transactTime(incremental.transact_time);

        // Add entries using the group API
        size_t num_entries = std::min(incremental.price_levels.size(), size_t(5));
        auto& entries_group = sbe_msg.noMDEntriesCount(static_cast<uint8_t>(num_entries));

        for (size_t i = 0; i < num_entries; ++i) {
            const auto& level = incremental.price_levels[i];

            // Use next() to advance to the next entry and chain field setters
            entries_group.next()
                .securityID(level.security_id)
                .rptSeq(level.rpt_seq)
                .mDPriceLevel(level.price_level)
                .numberOfOrders(level.number_of_orders)
                .mDUpdateAction(static_cast<cme_sbe::MDUpdateAction::Value>(level.update_action))
                .mDEntryType(static_cast<cme_sbe::MDEntryTypeBook::Value>(level.entry_type));

            // Set price using the PRICE9 composite
            entries_group.mDEntryPx().mantissa(level.price);

            // Set entry size - required field
            entries_group.mDEntrySize(level.quantity);
        }

        // Must encode NoOrderIDEntries group with count=0 following SBE pattern
        auto& order_entries_group = sbe_msg.noOrderIDEntriesCount(0);
        (void)order_entries_group; // No entries to populate

        // Get the total encoded length (header + message)
        size_t total_length = header_size + sbe_msg.encodedLength();
        buffer.resize(total_length);

        // Convert char buffer to uint8_t buffer
        return std::vector<uint8_t>(buffer.begin(), buffer.end());

    } catch (const std::exception& e) {
        LOG_ERROR("SBE incremental encoding error: " + std::string(e.what()));
        // Fall back to minimal message
        std::vector<uint8_t> minimal_buffer(64, 0);
        return minimal_buffer;
    }
}

void CMESBEEncoder::populate_snapshot_message(
    cme_sbe::SnapshotFullRefresh52& sbe_msg,
    const SnapshotFullRefresh& snapshot)
{
    // This will be implemented later - for now we do basic population in encode function
    (void)sbe_msg;
    (void)snapshot;
}

void CMESBEEncoder::populate_incremental_message(
    cme_sbe::MDIncrementalRefreshBook46& sbe_msg,
    const IncrementalRefresh& incremental)
{
    // This will be implemented later - for now we do basic population in encode function
    (void)sbe_msg;
    (void)incremental;
}

} // namespace cme_mock