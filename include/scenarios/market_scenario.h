#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <functional>

namespace cme_mock {

// Forward declarations
class OrderBook;
class MarketDataGenerator;

// Market scenario types
enum class ScenarioType {
    Normal,      // Regular trading conditions
    FastMarket,  // High frequency, rapid price movements
    ThinMarket,  // Low liquidity, wide spreads
    Recovery,    // Simulates connection loss and recovery
    Volatile,    // High volatility with price jumps
    Opening,     // Market opening with price discovery
    Closing      // Market closing with reduced activity
};

// Scenario configuration
struct ScenarioConfig {
    ScenarioType type = ScenarioType::Normal;
    std::string name;
    std::string description;
    
    // Market characteristics
    double volatility_multiplier = 1.0;      // Multiplier for price volatility
    double spread_multiplier = 1.0;          // Multiplier for bid-ask spread
    double volume_multiplier = 1.0;          // Multiplier for trade volume
    double update_frequency_multiplier = 1.0; // Multiplier for update rate
    
    // Book depth configuration
    uint32_t min_book_levels = 5;           // Minimum book depth
    uint32_t max_book_levels = 10;          // Maximum book depth
    uint32_t orders_per_level_min = 1;      // Min orders per price level
    uint32_t orders_per_level_max = 5;      // Max orders per price level
    
    // Price movement parameters
    double max_price_move_percent = 0.1;    // Max price move per update (%)
    double trend_strength = 0.0;            // -1.0 to 1.0 (negative = down, positive = up)
    uint32_t price_jump_probability = 0;    // Probability of price jumps (0-100)
    
    // Recovery scenario specific
    uint32_t gap_interval_seconds = 30;     // How often to simulate gaps
    uint32_t gap_duration_ms = 5000;        // Duration of simulated outage
    uint32_t messages_per_gap = 100;        // Messages lost during gap
    
    // Timing
    std::chrono::milliseconds base_update_interval{100}; // Base time between updates
    std::chrono::seconds duration{0};                    // Scenario duration (0 = infinite)
};

// Market event for scenario simulation
struct MarketEvent {
    enum Type {
        PriceUpdate,
        VolumeSpike,
        SpreadWiden,
        ConnectionLoss,
        SequenceGap,
        TradingHalt,
        TradingResume
    };
    
    Type type;
    std::chrono::system_clock::time_point scheduled_time;
    std::function<void()> action;
};

// Base class for market scenarios
class MarketScenario {
public:
    explicit MarketScenario(const ScenarioConfig& config);
    virtual ~MarketScenario() = default;
    
    // Lifecycle
    virtual void initialize() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;
    
    // Update the market state
    virtual void update() = 0;
    
    // Get current configuration
    const ScenarioConfig& config() const { return config_; }
    ScenarioType type() const { return config_.type; }
    const std::string& name() const { return config_.name; }
    
    // Check if scenario is active
    bool is_active() const { return active_; }
    
    // Get progress (0.0 to 1.0, or -1 if infinite)
    double get_progress() const;
    
protected:
    ScenarioConfig config_;
    bool active_ = false;
    std::chrono::system_clock::time_point start_time_;
    std::chrono::system_clock::time_point last_update_;
    
    // Schedule an event
    void schedule_event(const MarketEvent& event);
    
    // Process scheduled events
    void process_events();
    
    // Clear all scheduled events
    void clear_events() { scheduled_events_.clear(); }
    
private:
    std::vector<MarketEvent> scheduled_events_;
};

// Normal market conditions
class NormalMarketScenario : public MarketScenario {
public:
    explicit NormalMarketScenario(const ScenarioConfig& config);
    
    void initialize() override;
    void start() override;
    void stop() override;
    void reset() override;
    void update() override;
    
private:
    void generate_normal_activity();
};

// Fast market with high frequency updates
class FastMarketScenario : public MarketScenario {
public:
    explicit FastMarketScenario(const ScenarioConfig& config);
    
    void initialize() override;
    void start() override;
    void stop() override;
    void reset() override;
    void update() override;
    
private:
    void generate_rapid_updates();
    uint32_t burst_counter_ = 0;
};

// Thin market with low liquidity
class ThinMarketScenario : public MarketScenario {
public:
    explicit ThinMarketScenario(const ScenarioConfig& config);
    
    void initialize() override;
    void start() override;
    void stop() override;
    void reset() override;
    void update() override;
    
private:
    void generate_sparse_activity();
    void widen_spreads();
};

// Recovery scenario with gaps and reconnection
class RecoveryScenario : public MarketScenario {
public:
    explicit RecoveryScenario(const ScenarioConfig& config);
    
    void initialize() override;
    void start() override;
    void stop() override;
    void reset() override;
    void update() override;
    
private:
    void simulate_connection_loss();
    void simulate_recovery();
    void create_sequence_gap();
    
    bool in_gap_ = false;
    std::chrono::system_clock::time_point next_gap_time_;
    std::chrono::system_clock::time_point gap_end_time_;
    uint32_t messages_lost_ = 0;
};

// Scenario manager to coordinate scenarios
class ScenarioManager {
public:
    static ScenarioManager& instance();
    
    // Create scenario by type
    std::unique_ptr<MarketScenario> create_scenario(ScenarioType type);
    std::unique_ptr<MarketScenario> create_scenario(const ScenarioConfig& config);
    
    // Get available scenarios
    std::vector<ScenarioType> get_available_scenarios() const;
    std::string get_scenario_name(ScenarioType type) const;
    std::string get_scenario_description(ScenarioType type) const;
    
    // Get default configuration for a scenario type
    ScenarioConfig get_default_config(ScenarioType type) const;
    
    // Parse scenario type from string
    ScenarioType parse_scenario_type(const std::string& str) const;
    
private:
    ScenarioManager() = default;
};

} // namespace cme_mock