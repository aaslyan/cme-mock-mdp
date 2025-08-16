#include "scenarios/market_scenario.h"
#include "utils/logger.h"
#include <algorithm>
#include <sstream>

namespace cme_mock {

// MarketScenario base implementation
MarketScenario::MarketScenario(const ScenarioConfig& config)
    : config_(config)
{
}

double MarketScenario::get_progress() const
{
    if (config_.duration.count() == 0) {
        return -1.0; // Infinite duration
    }

    if (!active_) {
        return 0.0;
    }

    auto elapsed = std::chrono::system_clock::now() - start_time_;
    auto progress = static_cast<double>(elapsed.count()) / config_.duration.count();
    return std::min(1.0, progress);
}

void MarketScenario::schedule_event(const MarketEvent& event)
{
    scheduled_events_.push_back(event);
    // Keep events sorted by scheduled time
    std::sort(scheduled_events_.begin(), scheduled_events_.end(),
        [](const MarketEvent& a, const MarketEvent& b) {
            return a.scheduled_time < b.scheduled_time;
        });
}

void MarketScenario::process_events()
{
    auto now = std::chrono::system_clock::now();

    // Process all events that are due
    while (!scheduled_events_.empty() && scheduled_events_.front().scheduled_time <= now) {
        auto event = scheduled_events_.front();
        scheduled_events_.erase(scheduled_events_.begin());

        // Execute the event action
        if (event.action) {
            event.action();
        }
    }
}

// NormalMarketScenario implementation
NormalMarketScenario::NormalMarketScenario(const ScenarioConfig& config)
    : MarketScenario(config)
{
}

void NormalMarketScenario::initialize()
{
    Logger::instance().log(LogLevel::INFO, "Initializing Normal Market Scenario");

    // Set normal market characteristics
    config_.volatility_multiplier = 1.0;
    config_.spread_multiplier = 1.0;
    config_.volume_multiplier = 1.0;
    config_.update_frequency_multiplier = 1.0;
}

void NormalMarketScenario::start()
{
    Logger::instance().log(LogLevel::INFO, "Starting Normal Market Scenario");
    active_ = true;
    start_time_ = std::chrono::system_clock::now();
    last_update_ = start_time_;
}

void NormalMarketScenario::stop()
{
    Logger::instance().log(LogLevel::INFO, "Stopping Normal Market Scenario");
    active_ = false;
}

void NormalMarketScenario::reset()
{
    stop();
    clear_events();
}

void NormalMarketScenario::update()
{
    if (!active_)
        return;

    // Process any scheduled events
    process_events();

    // Generate normal market activity
    generate_normal_activity();

    last_update_ = std::chrono::system_clock::now();
}

void NormalMarketScenario::generate_normal_activity()
{
    // Normal market has steady, predictable activity
    // This would integrate with MarketDataGenerator to produce
    // regular price updates, moderate volume, normal spreads
}

// FastMarketScenario implementation
FastMarketScenario::FastMarketScenario(const ScenarioConfig& config)
    : MarketScenario(config)
{
}

void FastMarketScenario::initialize()
{
    Logger::instance().log(LogLevel::INFO, "Initializing Fast Market Scenario");

    // Fast market characteristics
    config_.volatility_multiplier = 2.5;
    config_.spread_multiplier = 0.8; // Tighter spreads
    config_.volume_multiplier = 3.0;
    config_.update_frequency_multiplier = 5.0; // 5x more updates
    config_.base_update_interval = std::chrono::milliseconds(20); // 50 updates/sec
}

void FastMarketScenario::start()
{
    Logger::instance().log(LogLevel::INFO, "Starting Fast Market Scenario");
    active_ = true;
    start_time_ = std::chrono::system_clock::now();
    last_update_ = start_time_;
    burst_counter_ = 0;
}

void FastMarketScenario::stop()
{
    Logger::instance().log(LogLevel::INFO, "Stopping Fast Market Scenario");
    active_ = false;
}

void FastMarketScenario::reset()
{
    stop();
    clear_events();
    burst_counter_ = 0;
}

void FastMarketScenario::update()
{
    if (!active_)
        return;

    process_events();
    generate_rapid_updates();

    last_update_ = std::chrono::system_clock::now();
}

void FastMarketScenario::generate_rapid_updates()
{
    // Generate bursts of activity
    burst_counter_++;

    // Every 10 updates, create a burst of 50 rapid updates
    if (burst_counter_ % 10 == 0) {
        Logger::instance().log(LogLevel::DEBUG, "Generating fast market burst");
        // This would trigger rapid order book changes
    }
}

// ThinMarketScenario implementation
ThinMarketScenario::ThinMarketScenario(const ScenarioConfig& config)
    : MarketScenario(config)
{
}

void ThinMarketScenario::initialize()
{
    Logger::instance().log(LogLevel::INFO, "Initializing Thin Market Scenario");

    // Thin market characteristics
    config_.volatility_multiplier = 1.5;
    config_.spread_multiplier = 3.0; // Wide spreads
    config_.volume_multiplier = 0.2; // Low volume
    config_.update_frequency_multiplier = 0.3; // Fewer updates
    config_.min_book_levels = 2;
    config_.max_book_levels = 5;
    config_.orders_per_level_min = 1;
    config_.orders_per_level_max = 2;
}

void ThinMarketScenario::start()
{
    Logger::instance().log(LogLevel::INFO, "Starting Thin Market Scenario");
    active_ = true;
    start_time_ = std::chrono::system_clock::now();
    last_update_ = start_time_;
}

void ThinMarketScenario::stop()
{
    Logger::instance().log(LogLevel::INFO, "Stopping Thin Market Scenario");
    active_ = false;
}

void ThinMarketScenario::reset()
{
    stop();
    clear_events();
}

void ThinMarketScenario::update()
{
    if (!active_)
        return;

    process_events();
    generate_sparse_activity();
    widen_spreads();

    last_update_ = std::chrono::system_clock::now();
}

void ThinMarketScenario::generate_sparse_activity()
{
    // Sparse, irregular updates
    // Large gaps between trades
    // Occasional order cancellations leaving gaps in book
}

void ThinMarketScenario::widen_spreads()
{
    // Periodically widen bid-ask spreads
    // Remove orders from inner price levels
}

// RecoveryScenario implementation
RecoveryScenario::RecoveryScenario(const ScenarioConfig& config)
    : MarketScenario(config)
{
}

void RecoveryScenario::initialize()
{
    Logger::instance().log(LogLevel::INFO, "Initializing Recovery Scenario");

    // Recovery scenario settings
    config_.gap_interval_seconds = 30;
    config_.gap_duration_ms = 5000;
    config_.messages_per_gap = 100;
}

void RecoveryScenario::start()
{
    Logger::instance().log(LogLevel::INFO, "Starting Recovery Scenario");
    active_ = true;
    start_time_ = std::chrono::system_clock::now();
    last_update_ = start_time_;

    // Schedule first gap
    next_gap_time_ = start_time_ + std::chrono::seconds(config_.gap_interval_seconds);
}

void RecoveryScenario::stop()
{
    Logger::instance().log(LogLevel::INFO, "Stopping Recovery Scenario");
    active_ = false;
}

void RecoveryScenario::reset()
{
    stop();
    clear_events();
    in_gap_ = false;
    messages_lost_ = 0;
}

void RecoveryScenario::update()
{
    if (!active_)
        return;

    auto now = std::chrono::system_clock::now();

    // Check if we should start a gap
    if (!in_gap_ && now >= next_gap_time_) {
        simulate_connection_loss();
    }

    // Check if gap should end
    if (in_gap_ && now >= gap_end_time_) {
        simulate_recovery();
    }

    process_events();
    last_update_ = now;
}

void RecoveryScenario::simulate_connection_loss()
{
    Logger::instance().log(LogLevel::INFO, "Simulating connection loss");

    in_gap_ = true;
    auto now = std::chrono::system_clock::now();
    gap_end_time_ = now + std::chrono::milliseconds(config_.gap_duration_ms);

    // Create sequence number gap
    create_sequence_gap();

    // Schedule next gap
    next_gap_time_ = gap_end_time_ + std::chrono::seconds(config_.gap_interval_seconds);
}

void RecoveryScenario::simulate_recovery()
{
    Logger::instance().log(LogLevel::INFO, "Simulating recovery - lost " + std::to_string(messages_lost_) + " messages");

    in_gap_ = false;
    messages_lost_ = 0;

    // This would trigger snapshot generation to help clients recover
}

void RecoveryScenario::create_sequence_gap()
{
    // This would advance sequence numbers without sending messages
    messages_lost_ = config_.messages_per_gap;
}

// ScenarioManager implementation
ScenarioManager& ScenarioManager::instance()
{
    static ScenarioManager instance;
    return instance;
}

std::unique_ptr<MarketScenario> ScenarioManager::create_scenario(ScenarioType type)
{
    auto config = get_default_config(type);
    return create_scenario(config);
}

std::unique_ptr<MarketScenario> ScenarioManager::create_scenario(const ScenarioConfig& config)
{
    switch (config.type) {
    case ScenarioType::Normal:
        return std::make_unique<NormalMarketScenario>(config);

    case ScenarioType::FastMarket:
        return std::make_unique<FastMarketScenario>(config);

    case ScenarioType::ThinMarket:
        return std::make_unique<ThinMarketScenario>(config);

    case ScenarioType::Recovery:
        return std::make_unique<RecoveryScenario>(config);

    default:
        Logger::instance().log(LogLevel::WARNING,
            "Unsupported scenario type, defaulting to Normal");
        return std::make_unique<NormalMarketScenario>(config);
    }
}

std::vector<ScenarioType> ScenarioManager::get_available_scenarios() const
{
    return {
        ScenarioType::Normal,
        ScenarioType::FastMarket,
        ScenarioType::ThinMarket,
        ScenarioType::Recovery,
        ScenarioType::Volatile,
        ScenarioType::Opening,
        ScenarioType::Closing
    };
}

std::string ScenarioManager::get_scenario_name(ScenarioType type) const
{
    switch (type) {
    case ScenarioType::Normal:
        return "normal";
    case ScenarioType::FastMarket:
        return "fast";
    case ScenarioType::ThinMarket:
        return "thin";
    case ScenarioType::Recovery:
        return "recovery";
    case ScenarioType::Volatile:
        return "volatile";
    case ScenarioType::Opening:
        return "opening";
    case ScenarioType::Closing:
        return "closing";
    default:
        return "unknown";
    }
}

std::string ScenarioManager::get_scenario_description(ScenarioType type) const
{
    switch (type) {
    case ScenarioType::Normal:
        return "Normal market conditions with regular trading activity";
    case ScenarioType::FastMarket:
        return "Fast market with high-frequency updates and rapid price movements";
    case ScenarioType::ThinMarket:
        return "Thin market with low liquidity and wide spreads";
    case ScenarioType::Recovery:
        return "Recovery scenario simulating connection loss and sequence gaps";
    case ScenarioType::Volatile:
        return "Volatile market with sudden price jumps";
    case ScenarioType::Opening:
        return "Market opening with price discovery";
    case ScenarioType::Closing:
        return "Market closing with reduced activity";
    default:
        return "Unknown scenario type";
    }
}

ScenarioConfig ScenarioManager::get_default_config(ScenarioType type) const
{
    ScenarioConfig config;
    config.type = type;
    config.name = get_scenario_name(type);
    config.description = get_scenario_description(type);

    // Type-specific defaults are set in each scenario's initialize()

    return config;
}

ScenarioType ScenarioManager::parse_scenario_type(const std::string& str) const
{
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "normal")
        return ScenarioType::Normal;
    if (lower == "fast")
        return ScenarioType::FastMarket;
    if (lower == "thin")
        return ScenarioType::ThinMarket;
    if (lower == "recovery")
        return ScenarioType::Recovery;
    if (lower == "volatile")
        return ScenarioType::Volatile;
    if (lower == "opening")
        return ScenarioType::Opening;
    if (lower == "closing")
        return ScenarioType::Closing;

    // Default to normal
    return ScenarioType::Normal;
}

} // namespace cme_mock