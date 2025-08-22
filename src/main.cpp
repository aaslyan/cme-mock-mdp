#include <atomic>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <thread>

#include "config/configuration.h"
#include "core/market_data_generator.h"
#include "core/order_book_manager.h"
#include "core/reference_data.h"
#include "network/feed_publisher.h"
#include "network/definition_feed_publisher.h"
#include "scenarios/market_scenario.h"
#include "utils/logger.h"

std::atomic<bool> g_running { true };

void signal_handler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutdown signal received..." << std::endl;
        g_running = false;
    }
}

void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -c, --config FILE      Configuration file (default: config/server_config.json)\n"
              << "  -s, --scenario TYPE    Market scenario: normal, fast, thin, recovery\n"
              << "  -l, --list-scenarios   List available scenarios\n"
              << "  -v, --verbose          Enable verbose logging\n"
              << "  -h, --help             Show this help message\n\n"
              << "Examples:\n"
              << "  " << program_name << " --scenario fast\n"
              << "  " << program_name << " --config custom.json --scenario recovery\n";
}

int main(int argc, char* argv[])
{
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Parse command line options
    std::string config_file = "config/server_config.json";
    std::string scenario_name;
    bool verbose = false;

    static struct option long_options[] = {
        { "config", required_argument, 0, 'c' },
        { "scenario", required_argument, 0, 's' },
        { "list-scenarios", no_argument, 0, 'l' },
        { "verbose", no_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:s:lvh", long_options, nullptr)) != -1) {
        switch (opt) {
        case 'c':
            config_file = optarg;
            break;
        case 's':
            scenario_name = optarg;
            break;
        case 'l': {
            auto& scenario_mgr = cme_mock::ScenarioManager::instance();
            std::cout << "Available market scenarios:\n";
            for (auto type : scenario_mgr.get_available_scenarios()) {
                std::cout << "  " << scenario_mgr.get_scenario_name(type)
                          << " - " << scenario_mgr.get_scenario_description(type) << "\n";
            }
            return 0;
        }
        case 'v':
            verbose = true;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    // Set logging level and global verbose flag
    extern bool g_verbose_mode;
    g_verbose_mode = verbose;
    
    if (verbose) {
        cme_mock::Logger::instance().set_level(cme_mock::LogLevel::DEBUG);
    } else {
        cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);
    }

    std::cout << "CME Mock MDP Server v1.0.0" << std::endl;
    std::cout << "==========================" << std::endl;

    auto& config = cme_mock::Configuration::instance();
    if (!config.load_from_file(config_file)) {
        std::cerr << "Failed to load configuration from: " << config_file << std::endl;
        return 1;
    }

    std::cout << "Configuration loaded successfully" << std::endl;
    std::cout << "Instruments: " << config.instruments().size() << std::endl;

    // Initialize reference data
    auto& ref_data = cme_mock::ReferenceDataManager::instance();
    if (!ref_data.initialize()) {
        std::cerr << "Failed to initialize Reference Data Manager" << std::endl;
        return 1;
    }

    // Create scenario based on command line or config
    auto& scenario_mgr = cme_mock::ScenarioManager::instance();
    std::unique_ptr<cme_mock::MarketScenario> scenario;

    if (!scenario_name.empty()) {
        // Use command line scenario
        auto scenario_type = scenario_mgr.parse_scenario_type(scenario_name);
        scenario = scenario_mgr.create_scenario(scenario_type);
        std::cout << "Scenario: " << scenario_name << " (from command line)" << std::endl;
    } else {
        // Use config file scenario - parse from legacy string
        auto scenario_type = scenario_mgr.parse_scenario_type(config.legacy_scenario().type);
        scenario = scenario_mgr.create_scenario(scenario_type);
        std::cout << "Scenario: " << config.legacy_scenario().type
                  << " (from config)" << std::endl;
    }

    scenario->initialize();

    // Initialize components
    try {
        // Create order book manager
        auto order_book_manager = std::make_shared<cme_mock::OrderBookManager>();

        // Initialize order books for all instruments
        for (const auto& instrument : config.instruments()) {
            order_book_manager->create_order_book(
                instrument.security_id,
                instrument.symbol,
                instrument.tick_size,
                instrument.initial_price,
                instrument.initial_spread);
            std::cout << "Created order book for " << instrument.symbol
                      << " (ID: " << instrument.security_id << ")" << std::endl;
        }

        // Create feed publishers
        auto snapshot_publisher = std::make_shared<cme_mock::SnapshotFeedPublisher>(
            config.network().snapshot_feed.ip,
            config.network().snapshot_feed.port,
            order_book_manager);

        auto incremental_publisher = std::make_shared<cme_mock::IncrementalFeedPublisher>(
            config.network().incremental_feed_a.ip,
            config.network().incremental_feed_a.port,
            order_book_manager);

        // Create definition feed publisher (use snapshot feed port + 1)
        auto definition_publisher = std::make_shared<cme_mock::DefinitionFeedPublisher>(
            config.network().snapshot_feed.ip,
            config.network().snapshot_feed.port + 1,
            std::shared_ptr<cme_mock::ReferenceDataManager>(&ref_data, [](cme_mock::ReferenceDataManager*) {}));

        // Initialize publishers
        if (!snapshot_publisher->initialize()) {
            std::cerr << "Failed to initialize snapshot publisher" << std::endl;
            return 1;
        }

        if (!incremental_publisher->initialize()) {
            std::cerr << "Failed to initialize incremental publisher" << std::endl;
            return 1;
        }

        if (!definition_publisher->initialize()) {
            std::cerr << "Failed to initialize definition publisher" << std::endl;
            return 1;
        }

        std::cout << "\nServer initialized successfully" << std::endl;
        std::cout << "Snapshot feed: " << config.network().snapshot_feed.ip
                  << ":" << config.network().snapshot_feed.port << std::endl;
        std::cout << "Incremental feed: " << config.network().incremental_feed_a.ip
                  << ":" << config.network().incremental_feed_a.port << std::endl;
        std::cout << "Definition feed: " << config.network().snapshot_feed.ip
                  << ":" << (config.network().snapshot_feed.port + 1) << std::endl;

        // Create market data generator
        auto market_generator = std::make_shared<cme_mock::MarketDataGenerator>(
            order_book_manager, incremental_publisher);

        // Configure market behavior based on scenario
        cme_mock::MarketConfig market_config;
        const auto& scenario_config = scenario->config();

        // Map scenario to market mode
        switch (scenario_config.type) {
        case cme_mock::ScenarioType::FastMarket:
            market_config.mode = cme_mock::MarketMode::Fast;
            break;
        case cme_mock::ScenarioType::ThinMarket:
            market_config.mode = cme_mock::MarketMode::Thin;
            break;
        case cme_mock::ScenarioType::Volatile:
            market_config.mode = cme_mock::MarketMode::Volatile;
            break;
        default:
            market_config.mode = cme_mock::MarketMode::Normal;
            break;
        }

        market_config.volatility = 0.0001 * scenario_config.volatility_multiplier;
        market_config.orders_per_second = config.legacy_scenario().message_rate * scenario_config.update_frequency_multiplier;
        market_config.trade_probability = 0.3;

        market_generator->set_config(market_config);

        std::cout << "\nServer is running. Press Ctrl+C to stop." << std::endl;
        std::cout << "Market Scenario: " << scenario->name() << std::endl;
        std::cout << "Generating " << market_config.orders_per_second << " updates/second" << std::endl;

        // Publish all instrument definitions at startup (like CME does)
        std::cout << "\nPublishing instrument definitions..." << std::endl;
        definition_publisher->publish_all_definitions();
        std::cout << "Instrument definitions published." << std::endl;

        // Start the scenario
        scenario->start();

        // Timer tracking
        auto last_snapshot_time = std::chrono::steady_clock::now();
        auto last_update_time = std::chrono::steady_clock::now();
        auto last_stats_time = std::chrono::steady_clock::now();
        auto last_scenario_update = std::chrono::steady_clock::now();

        auto snapshot_interval = std::chrono::seconds(config.market_data().snapshot_interval_seconds);
        auto update_interval = std::chrono::milliseconds(1000 / market_config.orders_per_second);
        auto stats_interval = std::chrono::seconds(10);
        auto scenario_update_interval = scenario_config.base_update_interval;

        // Main event loop
        while (g_running) {
            auto now = std::chrono::steady_clock::now();

            // Check if it's time for a snapshot
            if (now - last_snapshot_time >= snapshot_interval) {
                snapshot_publisher->publish_all_snapshots();
                last_snapshot_time = now;

                // Sync incremental sequence with snapshot
                incremental_publisher->set_sequence(snapshot_publisher->get_sequence());
            }

            // Update scenario state
            if (now - last_scenario_update >= scenario_update_interval) {
                scenario->update();
                last_scenario_update = now;
            }

            // Generate market updates
            if (now - last_update_time >= update_interval) {
                market_generator->generate_update();
                last_update_time = now;
            }

            // Print statistics periodically
            if (now - last_stats_time >= stats_interval) {
                std::cout << "Stats - Updates: " << market_generator->get_updates_generated()
                          << ", Trades: " << market_generator->get_trades_generated()
                          << ", Snapshots: " << snapshot_publisher->get_sequence()
                          << ", Incrementals: " << incremental_publisher->get_sequence()
                          << std::endl;
                last_stats_time = now;
            }

            // Sleep briefly to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Stop the scenario
        scenario->stop();

        std::cout << "\nFinal Statistics:" << std::endl;
        std::cout << "Total updates generated: " << market_generator->get_updates_generated() << std::endl;
        std::cout << "Total trades generated: " << market_generator->get_trades_generated() << std::endl;
        std::cout << "\nShutdown complete." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server stopped successfully" << std::endl;
    return 0;
}