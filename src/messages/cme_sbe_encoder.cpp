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
    std::vector<uint8_t> buffer(14); // 4 + 8 + 2 bytes: sequence + timestamp + message size

    // Encode sequence number (little-endian)
    buffer[0] = sequence_number & 0xFF;
    buffer[1] = (sequence_number >> 8) & 0xFF;
    buffer[2] = (sequence_number >> 16) & 0xFF;
    buffer[3] = (sequence_number >> 24) & 0xFF;

    // Encode sending time (little-endian)
    for (int i = 0; i < 8; ++i) {
        buffer[4 + i] = (sending_time >> (i * 8)) & 0xFF;
    }

    // Message size will be filled in later by the caller
    buffer[12] = 0;
    buffer[13] = 0;

    return buffer;
}

void CMESBEEncoder::set_message_size(std::vector<uint8_t>& packet, uint16_t message_size)
{
    // Set message size at offset 12-13 (little-endian)
    if (packet.size() >= 14) {
        packet[12] = message_size & 0xFF;
        packet[13] = (message_size >> 8) & 0xFF;
    }
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
    // Header: ~8 bytes, Message: ~30 bytes, NoMDEntries header: 8 bytes, NoOrderIDEntries header: 8 bytes, Entries: ~50 bytes each
    size_t estimated_entries = std::min(incremental.price_levels.size(), size_t(5));
    size_t buffer_size = 8 + 30 + 8 + 8 + (estimated_entries * 50) + 512; // Add padding for both groups
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
                .mDPriceLevel(level.price_level);

            // Set price using the PRICE9 composite
            entries_group.mDEntryPx().mantissa(level.price);
        }

        // Add NoOrderIDEntries group (empty for now - required by parser)
        auto& order_entries_group = sbe_msg.noOrderIDEntriesCount(0);
        (void)order_entries_group; // Suppress unused variable warning

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