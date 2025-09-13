#pragma once

#include "../../common/include/protocol_adapter.h"
#include "cme_messages.h"
#include <queue>

namespace cme_protocol {

// CME MDP 3.0 Protocol Adapter
class CMEProtocolAdapter : public market_protocols::IProtocolAdapter {
public:
    CMEProtocolAdapter();
    ~CMEProtocolAdapter() override = default;

    // Protocol identification
    std::string get_protocol_name() const override { return "CME MDP 3.0"; }
    std::string get_protocol_version() const override { return "3.0"; }

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
    bool supports_implied_prices() const override { return true; }
    bool supports_statistics() const override { return true; }
    size_t max_depth_levels() const override { return 10; }
    size_t max_message_size() const override { return 1400; } // UDP MTU

    // Transport
    void set_transport(std::shared_ptr<market_protocols::IMessageTransport> transport) override
    {
        transport_ = transport;
    }

    // CME-specific methods
    void set_channel_id(uint16_t channel_id) { channel_id_ = channel_id; }
    void set_batch_size(size_t size) { batch_size_ = size; }
    void flush_batch(); // Send accumulated messages

    // Sequence management
    uint32_t get_next_sequence() override { return ++sequence_number_; }
    void reset_sequence() override { sequence_number_ = 0; }

private:
    std::shared_ptr<market_protocols::IMessageTransport> transport_;
    uint16_t channel_id_ = 310; // Default CME Equity Futures channel
    uint32_t sequence_number_ = 0;
    size_t batch_size_ = 10;

    // Message batching
    std::queue<std::vector<uint8_t>> message_queue_;
    std::vector<std::vector<uint8_t>> current_batch_;

    // Helper methods
    std::vector<uint8_t> encode_incremental_refresh(
        const market_core::Instrument& instrument,
        const market_core::QuoteEvent& event);

    std::vector<uint8_t> encode_trade_summary(
        const market_core::Instrument& instrument,
        const market_core::TradeEvent& event);

    std::vector<uint8_t> encode_snapshot_full_refresh(
        const market_core::Instrument& instrument,
        const market_core::SnapshotEvent& event);

    std::vector<uint8_t> encode_statistics(
        const market_core::Instrument& instrument,
        const market_core::StatisticsEvent& event);

    std::vector<uint8_t> encode_security_status(
        const market_core::Instrument& instrument,
        const market_core::StatusEvent& event);

    std::vector<uint8_t> create_packet_header(uint64_t sending_time);
    std::vector<uint8_t> encode_message_size(uint16_t size);

    void send_message(const std::vector<uint8_t>& message);
    void send_batch(const std::vector<std::vector<uint8_t>>& messages);

    // Convert core types to CME types
    MDUpdateAction to_cme_update_action(market_core::UpdateAction action);
    MDEntryType to_cme_entry_type(market_core::Side side);
    uint32_t get_cme_security_id(const market_core::Instrument& instrument);
    int64_t price_to_cme(double price, const market_core::Instrument& instrument);
};

} // namespace cme_protocol