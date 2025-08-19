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
    uint64_t sending_time,
    uint16_t msg_count)
{
    std::vector<uint8_t> buffer(14); // 4 + 8 + 2 bytes for complete MDP header
    
    // Encode sequence number (little-endian)
    buffer[0] = sequence_number & 0xFF;
    buffer[1] = (sequence_number >> 8) & 0xFF;
    buffer[2] = (sequence_number >> 16) & 0xFF;
    buffer[3] = (sequence_number >> 24) & 0xFF;
    
    // Encode sending time (little-endian)
    for (int i = 0; i < 8; ++i) {
        buffer[4 + i] = (sending_time >> (i * 8)) & 0xFF;
    }
    
    // Encode msg_count (little-endian) at offset 12
    buffer[12] = msg_count & 0xFF;
    buffer[13] = (msg_count >> 8) & 0xFF;
    
    return buffer;
}

std::vector<uint8_t> CMESBEEncoder::encode_snapshot_full_refresh(
    const SnapshotFullRefresh& snapshot)
{
    // Calculate buffer size
    size_t buffer_size = 2048;
    std::vector<char> buffer(buffer_size);
    
    try {
        // Step 1: Encode the message header
        cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(), cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());
        header.blockLength(cme_sbe::SnapshotFullRefresh52::sbeBlockLength())
              .templateId(cme_sbe::SnapshotFullRefresh52::sbeTemplateId())
              .schemaId(cme_sbe::SnapshotFullRefresh52::sbeSchemaId())
              .version(cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());
        
        // Step 2: Encode the message body after the header
        size_t header_size = header.encodedLength();
        cme_sbe::SnapshotFullRefresh52 sbe_msg(
            buffer.data(), 
            header_size,
            buffer.size(),
            cme_sbe::SnapshotFullRefresh52::sbeBlockLength(),
            cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());
        
        // Set basic fields
        sbe_msg.securityID(snapshot.security_id);
        sbe_msg.lastMsgSeqNumProcessed(snapshot.last_msg_seq_num_processed);
        sbe_msg.rptSeq(snapshot.rpt_seq);
        sbe_msg.totNumReports(snapshot.tot_num_reports);
        sbe_msg.transactTime(snapshot.transact_time);
        
        // Add entries using the group API
        size_t num_entries = std::min(snapshot.entries.size(), size_t(5));
        auto& entries_group = sbe_msg.noMDEntriesCount(static_cast<uint16_t>(num_entries));
        
        for (size_t i = 0; i < num_entries; ++i) {
            const auto& entry = snapshot.entries[i];
            
            // Set basic entry fields
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
    // Calculate buffer size
    size_t buffer_size = 2048;
    std::vector<char> buffer(buffer_size);
    
    try {
        // Step 1: Encode the message header
        cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(), cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
        header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
              .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
              .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
              .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
        
        // Step 2: Encode the message body after the header
        size_t header_size = header.encodedLength();
        cme_sbe::MDIncrementalRefreshBook46 sbe_msg(
            buffer.data(), 
            header_size,
            buffer.size(),
            cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
            cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());
        
        // Set basic fields
        sbe_msg.transactTime(incremental.transact_time);
        
        // Add entries using the group API
        size_t num_entries = std::min(incremental.price_levels.size(), size_t(5));
        auto& entries_group = sbe_msg.noMDEntriesCount(static_cast<uint8_t>(num_entries));
        
        for (size_t i = 0; i < num_entries; ++i) {
            const auto& level = incremental.price_levels[i];
            
            // Set basic entry fields
            entries_group.next()
                .securityID(level.security_id)
                .rptSeq(level.rpt_seq)
                .mDPriceLevel(level.price_level);
            
            // Set price using the PRICE9 composite
            entries_group.mDEntryPx().mantissa(level.price);
        }
        
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