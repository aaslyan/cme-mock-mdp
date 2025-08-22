#pragma once

#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "cme_sbe/SnapshotFullRefresh52.h"
#include "mdp_messages.h"
#include <cstdint>
#include <vector>

namespace cme_mock {

// CME SBE Message Encoder using generated code
class CMESBEEncoder {
public:
    // Encode snapshot message using generated SBE code
    static std::vector<uint8_t> encode_snapshot_full_refresh(
        const SnapshotFullRefresh& snapshot);

    // Encode incremental refresh message using generated SBE code
    static std::vector<uint8_t> encode_incremental_refresh(
        const IncrementalRefresh& incremental);

    // Encode packet header (sequence number + timestamp)
    static std::vector<uint8_t> encode_packet_header(
        uint32_t sequence_number,
        uint64_t sending_time);

    // Encode message size field (2 bytes) that goes before each SBE message
    static std::vector<uint8_t> encode_message_size(uint16_t message_size);

    // Encode complete multi-message packet
    static std::vector<uint8_t> encode_multi_message_packet(
        uint32_t sequence_number,
        uint64_t sending_time,
        const std::vector<std::vector<uint8_t>>& messages);

private:
    // Helper to determine if instrument is FX (Channel 330)
    static bool is_fx_instrument(uint32_t security_id);

    // Convert our internal structures to SBE format
    static void populate_snapshot_message(
        cme_sbe::SnapshotFullRefresh52& sbe_msg,
        const SnapshotFullRefresh& snapshot);

    static void populate_incremental_message(
        cme_sbe::MDIncrementalRefreshBook46& sbe_msg,
        const IncrementalRefresh& incremental);
};

} // namespace cme_mock