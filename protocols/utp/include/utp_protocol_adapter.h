#pragma once

#include "../../common/include/protocol_adapter.h"
#include "utp_sbe/utp_sbe/MessageHeader.h"
#include "utp_sbe/utp_sbe/AdminHeartbeat.h"
#include "utp_sbe/utp_sbe/SecurityDefinition.h"
#include "utp_sbe/utp_sbe/MDFullRefresh.h"
#include "utp_sbe/utp_sbe/MDIncrementalRefresh.h"
#include <memory>
#include <vector>
#include <chrono>

namespace utp_protocol {

class UTPProtocolAdapter : public market_protocols::IProtocolAdapter {
private:
    std::shared_ptr<market_protocols::IMessageTransport> m_transport;
    uint16_t m_channel_id;
    size_t m_batch_size;
    uint64_t m_sequence_number;
    std::vector<uint8_t> m_send_buffer;

public:
    UTPProtocolAdapter();
    virtual ~UTPProtocolAdapter() = default;

    // Protocol identification
    std::string get_protocol_name() const override { return "UTP"; }
    std::string get_protocol_version() const override { return "1.0"; }

    // Event handling
    void process_quote_event(
        const market_core::Instrument& instrument,
        const market_core::QuoteEvent& event) override;

    void process_trade_event(
        const market_core::Instrument& instrument,
        const market_core::TradeEvent& event) override;

    void process_snapshot_event(
        const market_core::Instrument& instrument,
        const market_core::SnapshotEvent& event) override;

    void process_statistics_event(
        const market_core::Instrument& instrument,
        const market_core::StatisticsEvent& event) override;

    void process_status_event(
        const market_core::Instrument& instrument,
        const market_core::StatusEvent& event) override;

    void send_instrument_definition(
        const market_core::Instrument& instrument) override;

    // Protocol capabilities
    bool supports_incremental_updates() const override { return true; }
    bool supports_market_depth() const override { return true; }
    bool supports_implied_prices() const override { return false; }
    bool supports_statistics() const override { return false; }
    size_t max_depth_levels() const override { return 10; }
    size_t max_message_size() const override { return 1400; } // UDP MTU

    // Transport configuration
    void set_transport(std::shared_ptr<market_protocols::IMessageTransport> transport) override;

    // Sequence management
    uint32_t get_next_sequence() override { return static_cast<uint32_t>(++m_sequence_number); }
    void reset_sequence() override { m_sequence_number = 0; }

    // UTP-specific configuration
    void set_channel_id(uint16_t channel_id) { m_channel_id = channel_id; }
    void set_batch_size(size_t batch_size) { m_batch_size = batch_size; }
    uint64_t get_sequence_number() const { return m_sequence_number; }

private:
    // UTP message creation helpers
    bool create_heartbeat_message(std::vector<uint8_t>& buffer);
    bool create_security_definition(const market_core::Instrument& instrument, std::vector<uint8_t>& buffer);
    bool create_full_refresh_from_snapshot(const market_core::SnapshotEvent& snapshot, std::vector<uint8_t>& buffer);
    bool create_incremental_from_quote(const market_core::QuoteEvent& quote, std::vector<uint8_t>& buffer);

    // Message sending helpers
    bool send_utp_message(const std::vector<uint8_t>& message_data);
    uint64_t get_current_timestamp_ns();
    
    // SBE encoding helpers
    void encode_message_header(utp_sbe::MessageHeader& header, uint16_t template_id, uint16_t block_length);
    void set_instrument_string(char* dest, const std::string& src, size_t max_length);
};

} // namespace utp_protocol