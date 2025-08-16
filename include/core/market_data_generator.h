#pragma once

#include "network/feed_publisher.h"
#include "order_book_manager.h"
#include <chrono>
#include <memory>
#include <random>

namespace cme_mock {

enum class MarketMode {
    Normal,
    Fast,
    Thin,
    Volatile,
    Trending
};

struct MarketConfig {
    MarketMode mode = MarketMode::Normal;
    double volatility = 0.0001; // 0.01% price movement
    double spread_factor = 1.0; // Multiplier for spread width
    int orders_per_second = 10; // Rate of order generation
    double trade_probability = 0.3; // Chance of trade on each update
    double trend_bias = 0.0; // Directional bias (-1 to 1)
};

class MarketDataGenerator {
public:
    MarketDataGenerator(std::shared_ptr<OrderBookManager> book_manager,
        std::shared_ptr<IncrementalFeedPublisher> inc_publisher);

    // Generate a single market update
    void generate_update();

    // Generate multiple updates
    void generate_batch(int count);

    // Set market configuration
    void set_config(const MarketConfig& config) { config_ = config; }
    const MarketConfig& get_config() const { return config_; }

    // Statistics
    uint64_t get_updates_generated() const { return updates_generated_; }
    uint64_t get_trades_generated() const { return trades_generated_; }

private:
    std::shared_ptr<OrderBookManager> book_manager_;
    std::shared_ptr<IncrementalFeedPublisher> inc_publisher_;
    MarketConfig config_;

    // Random number generation
    std::mt19937 rng_;
    std::uniform_real_distribution<> uniform_dist_ { 0.0, 1.0 };
    std::normal_distribution<> normal_dist_ { 0.0, 1.0 };

    // Statistics
    uint64_t updates_generated_ = 0;
    uint64_t trades_generated_ = 0;
    uint32_t rpt_seq_ = 1;

    // Helper methods
    void generate_book_update(uint32_t security_id);
    void generate_trade(uint32_t security_id);
    double calculate_price_movement(double current_price);
    int calculate_quantity();
    bool should_generate_trade();
};

} // namespace cme_mock