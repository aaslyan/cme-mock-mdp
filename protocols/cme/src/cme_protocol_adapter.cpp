#include "../include/cme_protocol_adapter.h"
#include "../include/cme_encoder.h"
#include <chrono>

namespace cme_protocol {

CMEProtocolAdapter::CMEProtocolAdapter()
    : channel_id_(310)
    , sequence_number_(0)
    , batch_size_(10)
{
}

void CMEProtocolAdapter::process_quote_event(
    const market_core::Instrument& instrument,
    const market_core::QuoteEvent& event)
{
    // Convert core event to CME incremental refresh
    IncrementalRefresh refresh;
    refresh.transact_time = event.timestamp_ns;

    // Set match event indicator
    refresh.match_event_indicator.end_of_event = true;
    refresh.match_event_indicator.last_quote_msg = true;

    // Convert quote to CME price level
    MDPriceLevel level;
    level.security_id = get_cme_security_id(instrument);
    level.rpt_seq = event.rpt_seq.value_or(event.sequence_number);
    level.price = price_to_cme(event.price, instrument);
    level.quantity = static_cast<int32_t>(event.quantity);
    level.number_of_orders = event.order_count;
    level.price_level = event.price_level.value_or(1);
    level.update_action = to_cme_update_action(event.action);
    level.entry_type = to_cme_entry_type(event.side);
    level.tradeable_size = static_cast<int32_t>(event.quantity); // Usually same as quantity

    refresh.entries.push_back(level);

    // Encode and send
    auto encoded = encode_incremental_refresh(instrument, event);
    send_message(encoded);
}

void CMEProtocolAdapter::process_trade_event(
    const market_core::Instrument& instrument,
    const market_core::TradeEvent& event)
{
    auto encoded = encode_trade_summary(instrument, event);
    send_message(encoded);
}

void CMEProtocolAdapter::process_snapshot_event(
    const market_core::Instrument& instrument,
    const market_core::SnapshotEvent& event)
{
    auto encoded = encode_snapshot_full_refresh(instrument, event);
    send_message(encoded);
}

void CMEProtocolAdapter::process_statistics_event(
    const market_core::Instrument& instrument,
    const market_core::StatisticsEvent& event)
{
    auto encoded = encode_statistics(instrument, event);
    send_message(encoded);
}

void CMEProtocolAdapter::process_status_event(
    const market_core::Instrument& instrument,
    const market_core::StatusEvent& event)
{
    auto encoded = encode_security_status(instrument, event);
    send_message(encoded);
}

void CMEProtocolAdapter::send_instrument_definition(
    const market_core::Instrument& instrument)
{
    // Create security definition based on instrument type
    SecurityDefinition definition;
    definition.security_id = get_cme_security_id(instrument);
    definition.symbol = instrument.primary_symbol;
    definition.update_action = SecurityUpdateAction::Add;
    definition.min_price_increment = price_to_cme(instrument.tick_size, instrument);
    definition.display_factor = 1;
    definition.currency = instrument.get_property<std::string>("currency").value_or("USD");
    definition.security_exchange = "CME";

    // Set instrument-specific fields
    if (instrument.get_type() == market_core::InstrumentType::FUTURE) {
        if (auto futures = dynamic_cast<const market_core::FuturesInstrument*>(&instrument)) {
            definition.security_group = futures->underlying;
            definition.asset = futures->underlying;
            // Convert maturity date format if needed
            definition.maturity_month_year = futures->maturity_date;
        }
    }

    auto encoded = CMEEncoder::encode_security_definition_future(definition);
    send_message(encoded);
}

void CMEProtocolAdapter::flush_batch()
{
    if (current_batch_.empty()) {
        return;
    }

    send_batch(current_batch_);
    current_batch_.clear();
}

std::vector<uint8_t> CMEProtocolAdapter::encode_incremental_refresh(
    const market_core::Instrument& instrument,
    const market_core::QuoteEvent& event)
{
    IncrementalRefresh refresh;
    refresh.transact_time = event.timestamp_ns;

    // Set match event indicator
    refresh.match_event_indicator.end_of_event = true;
    refresh.match_event_indicator.last_quote_msg = true;

    // Create price level entry
    MDPriceLevel level;
    level.security_id = get_cme_security_id(instrument);
    level.rpt_seq = event.rpt_seq.value_or(event.sequence_number);
    level.price = price_to_cme(event.price, instrument);
    level.quantity = static_cast<int32_t>(event.quantity);
    level.number_of_orders = event.order_count;
    level.price_level = event.price_level.value_or(1);
    level.update_action = to_cme_update_action(event.action);
    level.entry_type = to_cme_entry_type(event.side);
    level.tradeable_size = static_cast<int32_t>(event.quantity);

    refresh.entries.push_back(level);

    return CMEEncoder::encode_incremental_refresh_book(refresh);
}

std::vector<uint8_t> CMEProtocolAdapter::encode_trade_summary(
    const market_core::Instrument& instrument,
    const market_core::TradeEvent& event)
{
    // For now, encode trade as incremental refresh with trade entry type
    IncrementalRefresh refresh;
    refresh.transact_time = event.timestamp_ns;

    refresh.match_event_indicator.end_of_event = true;
    refresh.match_event_indicator.last_trade_msg = true;

    MDPriceLevel trade_entry;
    trade_entry.security_id = get_cme_security_id(instrument);
    trade_entry.rpt_seq = event.rpt_seq.value_or(event.sequence_number);
    trade_entry.price = price_to_cme(event.price, instrument);
    trade_entry.quantity = static_cast<int32_t>(event.quantity);
    trade_entry.number_of_orders = 1;
    trade_entry.price_level = 1;
    trade_entry.update_action = MDUpdateAction::New;
    trade_entry.entry_type = MDEntryType::Trade;
    trade_entry.tradeable_size = 0; // N/A for trades

    refresh.entries.push_back(trade_entry);

    return CMEEncoder::encode_incremental_refresh_book(refresh);
}

std::vector<uint8_t> CMEProtocolAdapter::encode_snapshot_full_refresh(
    const market_core::Instrument& instrument,
    const market_core::SnapshotEvent& event)
{
    SnapshotFullRefresh snapshot;
    snapshot.security_id = get_cme_security_id(instrument);
    snapshot.transact_time = event.timestamp_ns;
    snapshot.rpt_seq = event.rpt_seq.value_or(event.sequence_number);

    // Convert bid levels
    for (const auto& bid : event.bid_levels) {
        MDPriceLevel level;
        level.security_id = snapshot.security_id;
        level.rpt_seq = bid.rpt_seq.value_or(bid.sequence_number);
        level.price = price_to_cme(bid.price, instrument);
        level.quantity = static_cast<int32_t>(bid.quantity);
        level.number_of_orders = bid.order_count;
        level.price_level = bid.price_level.value_or(static_cast<uint8_t>(snapshot.bid_entries.size() + 1));
        level.update_action = MDUpdateAction::New;
        level.entry_type = MDEntryType::Bid;
        level.tradeable_size = static_cast<int32_t>(bid.quantity);

        snapshot.bid_entries.push_back(level);
    }

    // Convert ask levels
    for (const auto& ask : event.ask_levels) {
        MDPriceLevel level;
        level.security_id = snapshot.security_id;
        level.rpt_seq = ask.rpt_seq.value_or(ask.sequence_number);
        level.price = price_to_cme(ask.price, instrument);
        level.quantity = static_cast<int32_t>(ask.quantity);
        level.number_of_orders = ask.order_count;
        level.price_level = ask.price_level.value_or(static_cast<uint8_t>(snapshot.ask_entries.size() + 1));
        level.update_action = MDUpdateAction::New;
        level.entry_type = MDEntryType::Offer;
        level.tradeable_size = static_cast<int32_t>(ask.quantity);

        snapshot.ask_entries.push_back(level);
    }

    // Set last trade price if available
    if (event.last_trade_price) {
        snapshot.last_trade_price = *event.last_trade_price;
    }

    // Set total volume if available
    if (event.total_volume) {
        snapshot.total_volume = *event.total_volume;
    }

    return CMEEncoder::encode_snapshot_full_refresh(snapshot);
}

std::vector<uint8_t> CMEProtocolAdapter::encode_statistics(
    const market_core::Instrument& instrument,
    const market_core::StatisticsEvent& event)
{
    // Encode as incremental refresh with statistics entry
    IncrementalRefresh refresh;
    refresh.transact_time = event.timestamp_ns;

    refresh.match_event_indicator.end_of_event = true;
    refresh.match_event_indicator.last_stats_msg = true;

    MDPriceLevel stats_entry;
    stats_entry.security_id = get_cme_security_id(instrument);
    stats_entry.rpt_seq = event.sequence_number;
    stats_entry.price = price_to_cme(event.value, instrument);
    stats_entry.quantity = event.volume ? static_cast<int32_t>(*event.volume) : 0;
    stats_entry.number_of_orders = 0;
    stats_entry.price_level = 1;
    stats_entry.update_action = MDUpdateAction::New;

    // Map statistics type to CME entry type
    switch (event.stat_type) {
    case market_core::StatisticsEvent::OPEN:
        stats_entry.entry_type = MDEntryType::OpeningPrice;
        break;
    case market_core::StatisticsEvent::HIGH:
        stats_entry.entry_type = MDEntryType::TradingSessionHighPrice;
        break;
    case market_core::StatisticsEvent::LOW:
        stats_entry.entry_type = MDEntryType::TradingSessionLowPrice;
        break;
    case market_core::StatisticsEvent::CLOSE:
        stats_entry.entry_type = MDEntryType::ClosingPrice;
        break;
    case market_core::StatisticsEvent::SETTLEMENT:
        stats_entry.entry_type = MDEntryType::SettlementPrice;
        break;
    case market_core::StatisticsEvent::TRADE_VOLUME:
        stats_entry.entry_type = MDEntryType::TradeVolume;
        break;
    default:
        stats_entry.entry_type = MDEntryType::ClosingPrice;
        break;
    }

    refresh.entries.push_back(stats_entry);

    return CMEEncoder::encode_incremental_refresh_book(refresh);
}

std::vector<uint8_t> CMEProtocolAdapter::encode_security_status(
    const market_core::Instrument& instrument,
    const market_core::StatusEvent& event)
{
    // Map core status to CME trading status
    uint8_t trading_status = 2; // Default: Open

    switch (event.status) {
    case market_core::StatusEvent::PRE_OPEN:
        trading_status = 21; // Pre-open
        break;
    case market_core::StatusEvent::OPENING_AUCTION:
        trading_status = 4; // Opening auction
        break;
    case market_core::StatusEvent::CONTINUOUS_TRADING:
        trading_status = 2; // Open
        break;
    case market_core::StatusEvent::CLOSING_AUCTION:
        trading_status = 5; // Closing auction
        break;
    case market_core::StatusEvent::POST_CLOSE:
        trading_status = 3; // Closed
        break;
    case market_core::StatusEvent::HALTED:
        trading_status = 12; // Halted
        break;
    case market_core::StatusEvent::PAUSED:
        trading_status = 13; // Paused
        break;
    }

    return CMEEncoder::encode_security_status(
        get_cme_security_id(instrument),
        trading_status,
        event.timestamp_ns);
}

std::vector<uint8_t> CMEProtocolAdapter::create_packet_header(uint64_t sending_time)
{
    return CMEEncoder::encode_packet_header(get_next_sequence(), sending_time);
}

std::vector<uint8_t> CMEProtocolAdapter::encode_message_size(uint16_t size)
{
    return CMEEncoder::encode_message_size(size);
}

void CMEProtocolAdapter::send_message(const std::vector<uint8_t>& message)
{
    if (current_batch_.size() >= batch_size_) {
        flush_batch();
    }

    current_batch_.push_back(message);

    // Send immediately if transport doesn't support batching
    if (!transport_) {
        return;
    }

    // For single message sends, flush immediately
    flush_batch();
}

void CMEProtocolAdapter::send_batch(const std::vector<std::vector<uint8_t>>& messages)
{
    if (!transport_ || messages.empty()) {
        return;
    }

    // Create complete packet with CME header
    uint64_t sending_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                                .count();

    auto packet = CMEEncoder::create_packet(get_next_sequence(), sending_time, messages);
    transport_->send_message(packet);
}

MDUpdateAction CMEProtocolAdapter::to_cme_update_action(market_core::UpdateAction action)
{
    switch (action) {
    case market_core::UpdateAction::ADD:
        return MDUpdateAction::New;
    case market_core::UpdateAction::CHANGE:
        return MDUpdateAction::Change;
    case market_core::UpdateAction::DELETE:
        return MDUpdateAction::Delete;
    case market_core::UpdateAction::OVERLAY:
        return MDUpdateAction::Overlay;
    default:
        return MDUpdateAction::New;
    }
}

MDEntryType CMEProtocolAdapter::to_cme_entry_type(market_core::Side side)
{
    switch (side) {
    case market_core::Side::BID:
        return MDEntryType::Bid;
    case market_core::Side::ASK:
        return MDEntryType::Offer;
    default:
        return MDEntryType::Bid;
    }
}

uint32_t CMEProtocolAdapter::get_cme_security_id(const market_core::Instrument& instrument)
{
    // Try to get CME-specific security ID
    auto cme_id = instrument.get_external_id("CME_SECURITY_ID");
    if (cme_id) {
        return std::stoul(*cme_id);
    }

    // Fallback to internal instrument ID
    return instrument.instrument_id;
}

int64_t CMEProtocolAdapter::price_to_cme(double price, const market_core::Instrument& instrument)
{
    // Get price decimals from instrument properties, default to 2
    int decimals = instrument.get_property<int64_t>("price_decimals").value_or(2);

    double multiplier = 1.0;
    for (int i = 0; i < decimals; ++i) {
        multiplier *= 10.0;
    }

    return static_cast<int64_t>(price * multiplier);
}

} // namespace cme_protocol