#pragma once

#include "../../core/include/market_data_generator.h"
#include "../../core/include/order_book_manager.h"
#include "cme_protocol_adapter.h"
#include <memory>

namespace cme_protocol {

// Event listener that connects core market events to CME protocol adapter
class CMEEventListener : public market_core::IMarketEventListener {
public:
    CMEEventListener(
        std::shared_ptr<market_core::OrderBookManager> book_manager,
        std::shared_ptr<CMEProtocolAdapter> cme_adapter);

    ~CMEEventListener() override = default;

    // IMarketEventListener interface
    void on_market_event(const std::shared_ptr<market_core::MarketEvent>& event) override;

private:
    std::shared_ptr<market_core::OrderBookManager> book_manager_;
    std::shared_ptr<CMEProtocolAdapter> cme_adapter_;

    // Event dispatch methods
    void handle_quote_event(const std::shared_ptr<market_core::QuoteEvent>& event);
    void handle_trade_event(const std::shared_ptr<market_core::TradeEvent>& event);
    void handle_snapshot_event(const std::shared_ptr<market_core::SnapshotEvent>& event);
    void handle_statistics_event(const std::shared_ptr<market_core::StatisticsEvent>& event);
    void handle_status_event(const std::shared_ptr<market_core::StatusEvent>& event);
};

} // namespace cme_protocol