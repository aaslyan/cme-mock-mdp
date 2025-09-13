#pragma once

#include "cme_messages.h"
#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MDIncrementalRefreshSessionStatistics51.h"
#include "cme_sbe/MDIncrementalRefreshTradeSummary48.h"
#include "cme_sbe/MDInstrumentDefinitionFuture54.h"
#include "cme_sbe/MessageHeader.h"
#include "cme_sbe/SecurityStatus30.h"
#include "cme_sbe/SnapshotFullRefresh52.h"
#include <cstdint>
#include <vector>

namespace cme_protocol {

// CME SBE Message Encoder - uses SBE generated code for all encoding
class CMEEncoder {
public:
    // Buffer management
    static constexpr size_t MAX_MESSAGE_SIZE = 1400; // UDP MTU
    static constexpr size_t HEADER_SIZE = 8; // SBE message header

    // Encode incremental refresh book using SBE API
    // This method:
    // 1. Allocates a buffer
    // 2. Wraps it with SBE encoder
    // 3. Uses SBE API to set all fields
    // 4. Returns the encoded bytes
    static std::vector<uint8_t> encode_incremental_refresh_book(
        const IncrementalRefresh& refresh);

    // Encode snapshot using SBE API
    static std::vector<uint8_t> encode_snapshot_full_refresh(
        const SnapshotFullRefresh& snapshot);

    // Encode security definition using SBE API
    static std::vector<uint8_t> encode_security_definition_future(
        const SecurityDefinition& definition);

    // Encode security status using SBE API
    static std::vector<uint8_t> encode_security_status(
        uint32_t security_id,
        uint8_t trading_status,
        uint64_t transact_time);

    // Encode statistics using SBE API
    static std::vector<uint8_t> encode_session_statistics(
        const MDStatistics& stats);

    // Packet-level encoding (not SBE, but CME packet format)
    static std::vector<uint8_t> encode_packet_header(
        uint32_t sequence_number,
        uint64_t sending_time);

    static std::vector<uint8_t> encode_message_size(uint16_t message_size);

    // Create complete packet with multiple SBE messages
    static std::vector<uint8_t> create_packet(
        uint32_t sequence_number,
        uint64_t sending_time,
        const std::vector<std::vector<uint8_t>>& sbe_messages);

private:
    // Template method for SBE encoding pattern
    // All SBE encoding follows this pattern:
    // 1. Create buffer
    // 2. Wrap with MessageHeader
    // 3. Set header fields
    // 4. Wrap with specific message type
    // 5. Use fluent API to set fields
    // 6. Return encoded length
    template <typename SBEMessage>
    static size_t wrap_and_encode_header(
        uint8_t* buffer,
        size_t buffer_length,
        uint16_t template_id);

    // Helper to ensure we use SBE's encoding for all fields
    static void encode_using_sbe_api(
        cme_sbe::MDIncrementalRefreshBook46& sbe_msg,
        const IncrementalRefresh& refresh);

    static void encode_using_sbe_api(
        cme_sbe::SnapshotFullRefresh52& sbe_msg,
        const SnapshotFullRefresh& snapshot);

    static void encode_using_sbe_api(
        cme_sbe::MDInstrumentDefinitionFuture54& sbe_msg,
        const SecurityDefinition& definition);

    // Price conversion (but still let SBE encode the binary)
    static int64_t to_sbe_price(double price, int decimals = 2);
};

} // namespace cme_protocol