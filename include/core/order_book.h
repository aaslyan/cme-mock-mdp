#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace cme_mock {

struct PriceLevel {
    double price;
    uint64_t quantity;
    uint32_t order_count;
    uint64_t last_update_time;
};

struct Trade {
    double price;
    uint64_t quantity;
    uint64_t timestamp;
    char aggressor_side;
};

struct MarketStats {
    double open_price = 0.0;
    double high_price = 0.0;
    double low_price = 0.0;
    double last_price = 0.0;
    double settlement_price = 0.0;
    uint64_t total_volume = 0;
    uint32_t trade_count = 0;
    double vwap = 0.0;
};

class OrderBook {
public:
    OrderBook(uint32_t security_id, const std::string& symbol);
    ~OrderBook() = default;

    // Order book operations
    void add_bid(double price, uint64_t quantity);
    void add_ask(double price, uint64_t quantity);
    void update_bid(double price, uint64_t quantity);
    void update_ask(double price, uint64_t quantity);
    void remove_bid(double price);
    void remove_ask(double price);
    void clear();

    // Trade operations
    void add_trade(double price, uint64_t quantity, char aggressor_side);

    // Getters
    std::vector<PriceLevel> get_bids(size_t max_levels = 10) const;
    std::vector<PriceLevel> get_asks(size_t max_levels = 10) const;
    std::vector<Trade> get_recent_trades(size_t count = 10) const;

    double get_best_bid() const;
    double get_best_ask() const;
    double get_mid_price() const;
    double get_spread() const;

    const MarketStats& get_stats() const { return stats_; }
    uint32_t get_security_id() const { return security_id_; }
    const std::string& get_symbol() const { return symbol_; }

    // State
    bool is_crossed() const;
    size_t bid_depth() const;
    size_t ask_depth() const;

private:
    uint32_t security_id_;
    std::string symbol_;

    // Using map for automatic price ordering
    std::map<double, PriceLevel, std::greater<double>> bids_; // Descending order
    std::map<double, PriceLevel> asks_; // Ascending order

    std::vector<Trade> recent_trades_;
    MarketStats stats_;

    void update_stats_on_trade(double price, uint64_t quantity);
};

} // namespace cme_mock