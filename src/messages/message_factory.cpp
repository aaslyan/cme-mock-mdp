#include "messages/mdp_messages.h"
#include "messages/sbe_encoder.h"
#include "core/order_book.h"
#include <chrono>

namespace cme_mock {

// Helper function to get current timestamp in nanoseconds
uint64_t get_timestamp_ns() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

// Convert order book to snapshot message
SnapshotFullRefresh create_snapshot_from_book(
    const OrderBook& book,
    uint32_t last_seq_num,
    uint32_t rpt_seq) {
    
    SnapshotFullRefresh snapshot;
    snapshot.header.sequence_number = 0; // Set by publisher
    snapshot.header.sending_time = get_timestamp_ns();
    snapshot.header.msg_count = 1;
    
    snapshot.security_id = book.get_security_id();
    snapshot.last_msg_seq_num_processed = last_seq_num;
    snapshot.rpt_seq = rpt_seq;
    snapshot.tot_num_reports = 1;
    snapshot.security_trading_status = 2; // Trading
    snapshot.transact_time = get_timestamp_ns();
    
    // Add bid entries
    auto bids = book.get_bids(10);
    uint8_t level = 1;
    for (const auto& bid : bids) {
        MDSnapshotEntry entry;
        entry.entry_type = MDEntryType::Bid;
        entry.price = static_cast<int64_t>(bid.price * 100); // 2 decimals
        entry.quantity = static_cast<int32_t>(bid.quantity);
        entry.number_of_orders = bid.order_count;
        entry.price_level = level++;
        snapshot.entries.push_back(entry);
    }
    
    // Add ask entries
    auto asks = book.get_asks(10);
    level = 1;
    for (const auto& ask : asks) {
        MDSnapshotEntry entry;
        entry.entry_type = MDEntryType::Offer;
        entry.price = static_cast<int64_t>(ask.price * 100); // 2 decimals
        entry.quantity = static_cast<int32_t>(ask.quantity);
        entry.number_of_orders = ask.order_count;
        entry.price_level = level++;
        snapshot.entries.push_back(entry);
    }
    
    // Add trade entry if there's a last trade
    const auto& stats = book.get_stats();
    if (stats.last_price > 0) {
        MDSnapshotEntry trade_entry;
        trade_entry.entry_type = MDEntryType::Trade;
        trade_entry.price = static_cast<int64_t>(stats.last_price * 100);
        trade_entry.quantity = 0; // Not tracked in current implementation
        trade_entry.number_of_orders = 0;
        trade_entry.price_level = 0;
        snapshot.entries.push_back(trade_entry);
    }
    
    return snapshot;
}

// Create incremental update for a price level change
IncrementalRefresh create_price_level_update(
    uint32_t security_id,
    MDUpdateAction action,
    MDEntryType side,
    double price,
    int32_t quantity,
    uint8_t level,
    uint32_t rpt_seq) {
    
    IncrementalRefresh update;
    update.header.sequence_number = 0; // Set by publisher
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 1;
    update.transact_time = get_timestamp_ns();
    
    MDPriceLevel price_level;
    price_level.update_action = action;
    price_level.entry_type = side;
    price_level.security_id = security_id;
    price_level.rpt_seq = rpt_seq;
    price_level.price = static_cast<int64_t>(price * 100); // 2 decimals
    price_level.quantity = quantity;
    price_level.number_of_orders = 1; // Simplified
    price_level.price_level = level;
    
    update.price_levels.push_back(price_level);
    
    return update;
}

// Create incremental update for a trade
IncrementalRefresh create_trade_update(
    uint32_t security_id,
    double price,
    int32_t quantity,
    char aggressor_side,
    uint32_t rpt_seq) {
    
    IncrementalRefresh update;
    update.header.sequence_number = 0; // Set by publisher
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 1;
    update.transact_time = get_timestamp_ns();
    
    MDTrade trade;
    trade.security_id = security_id;
    trade.rpt_seq = rpt_seq;
    trade.price = static_cast<int64_t>(price * 100); // 2 decimals
    trade.quantity = quantity;
    trade.number_of_orders = 1; // Simplified
    trade.aggressor_side = aggressor_side;
    
    update.trades.push_back(trade);
    
    return update;
}

} // namespace cme_mock