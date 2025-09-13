#include "../include/cme_event_listener.h"

namespace cme_protocol {

CMEEventListener::CMEEventListener(
    std::shared_ptr<market_core::OrderBookManager> book_manager,
    std::shared_ptr<CMEProtocolAdapter> cme_adapter)
    : book_manager_(book_manager)
    , cme_adapter_(cme_adapter)
{
}

void CMEEventListener::on_market_event(const std::shared_ptr<market_core::MarketEvent>& event)
{
    if (!event || !cme_adapter_) {
        return;
    }

    // Dispatch based on event type
    switch (event->type) {
    case market_core::MarketEvent::QUOTE_UPDATE: {
        auto quote_event = std::static_pointer_cast<market_core::QuoteEvent>(event);
        handle_quote_event(quote_event);
        break;
    }
    case market_core::MarketEvent::TRADE: {
        auto trade_event = std::static_pointer_cast<market_core::TradeEvent>(event);
        handle_trade_event(trade_event);
        break;
    }
    case market_core::MarketEvent::SNAPSHOT: {
        auto snapshot_event = std::static_pointer_cast<market_core::SnapshotEvent>(event);
        handle_snapshot_event(snapshot_event);
        break;
    }
    case market_core::MarketEvent::STATISTICS: {
        auto stats_event = std::static_pointer_cast<market_core::StatisticsEvent>(event);
        handle_statistics_event(stats_event);
        break;
    }
    case market_core::MarketEvent::STATUS_CHANGE: {
        auto status_event = std::static_pointer_cast<market_core::StatusEvent>(event);
        handle_status_event(status_event);
        break;
    }
    default:
        // Ignore unknown event types
        break;
    }
}

void CMEEventListener::handle_quote_event(const std::shared_ptr<market_core::QuoteEvent>& event)
{
    // Get instrument for this event
    auto instrument = book_manager_->get_instrument(event->instrument_id);
    if (!instrument) {
        return;
    }

    // Forward to CME adapter
    cme_adapter_->process_quote_event(*instrument, *event);
}

void CMEEventListener::handle_trade_event(const std::shared_ptr<market_core::TradeEvent>& event)
{
    auto instrument = book_manager_->get_instrument(event->instrument_id);
    if (!instrument) {
        return;
    }

    cme_adapter_->process_trade_event(*instrument, *event);
}

void CMEEventListener::handle_snapshot_event(const std::shared_ptr<market_core::SnapshotEvent>& event)
{
    auto instrument = book_manager_->get_instrument(event->instrument_id);
    if (!instrument) {
        return;
    }

    cme_adapter_->process_snapshot_event(*instrument, *event);
}

void CMEEventListener::handle_statistics_event(const std::shared_ptr<market_core::StatisticsEvent>& event)
{
    auto instrument = book_manager_->get_instrument(event->instrument_id);
    if (!instrument) {
        return;
    }

    cme_adapter_->process_statistics_event(*instrument, *event);
}

void CMEEventListener::handle_status_event(const std::shared_ptr<market_core::StatusEvent>& event)
{
    auto instrument = book_manager_->get_instrument(event->instrument_id);
    if (!instrument) {
        return;
    }

    cme_adapter_->process_status_event(*instrument, *event);
}

} // namespace cme_protocol