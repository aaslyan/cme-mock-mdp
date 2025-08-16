#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cme_mock {

struct NetworkConfig {
    std::string mode = "unicast"; // unicast or multicast

    struct FeedConfig {
        std::string ip;
        uint16_t port;
    };

    FeedConfig incremental_feed_a = { "127.0.0.1", 14310 };
    FeedConfig incremental_feed_b = { "127.0.0.1", 14311 };
    FeedConfig snapshot_feed = { "127.0.0.1", 14320 };
    FeedConfig definition_feed = { "127.0.0.1", 14330 };
};

struct MarketDataConfig {
    uint32_t snapshot_interval_seconds = 30;
    uint32_t message_batch_size = 10;
    uint32_t max_book_depth = 10;
    uint32_t incremental_publish_interval_ms = 10;
};

struct InstrumentConfig {
    std::string symbol;
    uint32_t security_id;
    std::string name;
    std::string underlying;
    std::string exchange;
    std::string product_group;
    double tick_size;
    double multiplier;
    double initial_price;
    double initial_spread;
    uint8_t price_decimals;
    double contract_size;
    double initial_margin;
    double maintenance_margin;
    std::string maturity_date;
};

// Forward declaration - defined in scenarios/market_scenario.h
enum class ScenarioType;
struct ScenarioConfig;

// Legacy scenario config for backward compatibility
struct LegacyScenarioConfig {
    std::string type = "normal"; // normal, fast, thin, recovery
    std::string volatility = "medium"; // low, medium, high
    uint32_t message_rate = 100; // messages per second
    uint32_t duration_minutes = 60;

    // Market behavior parameters
    double price_drift = 0.0; // directional bias
    double spread_multiplier = 1.0; // wider spreads in fast markets
    uint32_t book_depth_target = 5; // target levels per side
};

class Configuration {
public:
    static Configuration& instance();

    bool load_from_file(const std::string& filename);
    bool load_from_json(const std::string& json_content);

    // Getters
    const NetworkConfig& network() const { return network_; }
    const MarketDataConfig& market_data() const { return market_data_; }
    const std::vector<InstrumentConfig>& instruments() const { return instruments_; }
    const LegacyScenarioConfig& legacy_scenario() const { return legacy_scenario_; }

    // Setters for runtime updates
    void set_legacy_scenario(const LegacyScenarioConfig& scenario) { legacy_scenario_ = scenario; }

private:
    Configuration() = default;

    NetworkConfig network_;
    MarketDataConfig market_data_;
    std::vector<InstrumentConfig> instruments_;
    LegacyScenarioConfig legacy_scenario_;
};

} // namespace cme_mock