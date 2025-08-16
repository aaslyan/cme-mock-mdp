#include "core/order_book.h"
#include "utils/logger.h"
#include <algorithm>
#include <sstream>

namespace cme_mock {

OrderBook::OrderBook(uint32_t security_id, const std::string& symbol)
    : security_id_(security_id)
    , symbol_(symbol)
{
    LOG_DEBUG("Created order book for " + symbol + " (ID: " + std::to_string(security_id) + ")");
}

void OrderBook::add_bid(double price, uint64_t quantity)
{
    auto& level = bids_[price];
    level.price = price;
    level.quantity += quantity;
    level.order_count++;
    level.last_update_time = std::chrono::steady_clock::now().time_since_epoch().count();

    LOG_DEBUG(symbol_ + " - Added bid: " + std::to_string(quantity) + " @ " + std::to_string(price));
}

void OrderBook::add_ask(double price, uint64_t quantity)
{
    auto& level = asks_[price];
    level.price = price;
    level.quantity += quantity;
    level.order_count++;
    level.last_update_time = std::chrono::steady_clock::now().time_since_epoch().count();

    LOG_DEBUG(symbol_ + " - Added ask: " + std::to_string(quantity) + " @ " + std::to_string(price));
}

void OrderBook::update_bid(double price, uint64_t quantity)
{
    auto it = bids_.find(price);
    if (it != bids_.end()) {
        it->second.quantity = quantity;
        it->second.last_update_time = std::chrono::steady_clock::now().time_since_epoch().count();

        if (quantity == 0) {
            bids_.erase(it);
            LOG_DEBUG(symbol_ + " - Removed bid level at " + std::to_string(price));
        }
    } else if (quantity > 0) {
        add_bid(price, quantity);
    }
}

void OrderBook::update_ask(double price, uint64_t quantity)
{
    auto it = asks_.find(price);
    if (it != asks_.end()) {
        it->second.quantity = quantity;
        it->second.last_update_time = std::chrono::steady_clock::now().time_since_epoch().count();

        if (quantity == 0) {
            asks_.erase(it);
            LOG_DEBUG(symbol_ + " - Removed ask level at " + std::to_string(price));
        }
    } else if (quantity > 0) {
        add_ask(price, quantity);
    }
}

void OrderBook::remove_bid(double price)
{
    bids_.erase(price);
}

void OrderBook::remove_ask(double price)
{
    asks_.erase(price);
}

void OrderBook::clear()
{
    bids_.clear();
    asks_.clear();
    recent_trades_.clear();
    LOG_INFO(symbol_ + " - Order book cleared");
}

void OrderBook::add_trade(double price, uint64_t quantity, char aggressor_side)
{
    Trade trade;
    trade.price = price;
    trade.quantity = quantity;
    trade.timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    trade.aggressor_side = aggressor_side;

    recent_trades_.push_back(trade);

    // Keep only recent trades (last 100)
    if (recent_trades_.size() > 100) {
        recent_trades_.erase(recent_trades_.begin());
    }

    // Update statistics
    update_stats_on_trade(price, quantity);

    LOG_DEBUG(symbol_ + " - Trade: " + std::to_string(quantity) + " @ " + std::to_string(price) + " (aggressor: " + aggressor_side + ")");
}

std::vector<PriceLevel> OrderBook::get_bids(size_t max_levels) const
{
    std::vector<PriceLevel> result;
    result.reserve(max_levels);

    size_t count = 0;
    for (const auto& [price, level] : bids_) {
        if (count >= max_levels)
            break;
        result.push_back(level);
        count++;
    }

    return result;
}

std::vector<PriceLevel> OrderBook::get_asks(size_t max_levels) const
{
    std::vector<PriceLevel> result;
    result.reserve(max_levels);

    size_t count = 0;
    for (const auto& [price, level] : asks_) {
        if (count >= max_levels)
            break;
        result.push_back(level);
        count++;
    }

    return result;
}

std::vector<Trade> OrderBook::get_recent_trades(size_t count) const
{
    std::vector<Trade> result;

    size_t start_idx = recent_trades_.size() > count ? recent_trades_.size() - count : 0;
    for (size_t i = start_idx; i < recent_trades_.size(); ++i) {
        result.push_back(recent_trades_[i]);
    }

    return result;
}

double OrderBook::get_best_bid() const
{
    if (bids_.empty())
        return 0.0;
    return bids_.begin()->first;
}

double OrderBook::get_best_ask() const
{
    if (asks_.empty())
        return 0.0;
    return asks_.begin()->first;
}

double OrderBook::get_mid_price() const
{
    double bid = get_best_bid();
    double ask = get_best_ask();

    if (bid == 0.0 || ask == 0.0)
        return 0.0;
    return (bid + ask) / 2.0;
}

double OrderBook::get_spread() const
{
    double bid = get_best_bid();
    double ask = get_best_ask();

    if (bid == 0.0 || ask == 0.0)
        return 0.0;
    return ask - bid;
}

bool OrderBook::is_crossed() const
{
    double bid = get_best_bid();
    double ask = get_best_ask();

    if (bid == 0.0 || ask == 0.0)
        return false;
    return bid >= ask;
}

size_t OrderBook::bid_depth() const
{
    return bids_.size();
}

size_t OrderBook::ask_depth() const
{
    return asks_.size();
}

void OrderBook::update_stats_on_trade(double price, uint64_t quantity)
{
    // Update last price
    stats_.last_price = price;

    // Update volume
    stats_.total_volume += quantity;
    stats_.trade_count++;

    // Update high/low
    if (stats_.high_price == 0.0 || price > stats_.high_price) {
        stats_.high_price = price;
    }
    if (stats_.low_price == 0.0 || price < stats_.low_price) {
        stats_.low_price = price;
    }

    // Update VWAP
    static double total_value = 0.0;
    total_value += price * quantity;
    if (stats_.total_volume > 0) {
        stats_.vwap = total_value / stats_.total_volume;
    }

    // Set open price on first trade
    if (stats_.open_price == 0.0) {
        stats_.open_price = price;
    }
}

} // namespace cme_mock