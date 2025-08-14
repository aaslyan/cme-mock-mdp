#include <iostream>
#include <signal.h>
#include <atomic>
#include <thread>
#include <chrono>

#include "config/configuration.h"
#include "core/order_book_manager.h"
#include "core/market_data_generator.h"
#include "network/feed_publisher.h"
#include "utils/logger.h"

std::atomic<bool> g_running{true};

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutdown signal received..." << std::endl;
        g_running = false;
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    std::cout << "CME Mock MDP Server v1.0.0" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Load configuration
    std::string config_file = "config/server_config.json";
    if (argc > 1) {
        config_file = argv[1];
    }
    
    auto& config = cme_mock::Configuration::instance();
    if (!config.load_from_file(config_file)) {
        std::cerr << "Failed to load configuration from: " << config_file << std::endl;
        return 1;
    }
    
    std::cout << "Configuration loaded successfully" << std::endl;
    std::cout << "Instruments: " << config.instruments().size() << std::endl;
    std::cout << "Scenario: " << static_cast<int>(config.scenario().type) << std::endl;
    
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
                instrument.initial_spread
            );
            std::cout << "Created order book for " << instrument.symbol 
                      << " (ID: " << instrument.security_id << ")" << std::endl;
        }
        
        // Create feed publishers
        auto snapshot_publisher = std::make_shared<cme_mock::SnapshotFeedPublisher>(
            config.network().snapshot_feed.ip,
            config.network().snapshot_feed.port,
            order_book_manager
        );
        
        auto incremental_publisher = std::make_shared<cme_mock::IncrementalFeedPublisher>(
            config.network().incremental_feed_a.ip,
            config.network().incremental_feed_a.port,
            order_book_manager
        );
        
        // Initialize publishers
        if (!snapshot_publisher->initialize()) {
            std::cerr << "Failed to initialize snapshot publisher" << std::endl;
            return 1;
        }
        
        if (!incremental_publisher->initialize()) {
            std::cerr << "Failed to initialize incremental publisher" << std::endl;
            return 1;
        }
        
        std::cout << "\nServer initialized successfully" << std::endl;
        std::cout << "Snapshot feed: " << config.network().snapshot_feed.ip 
                  << ":" << config.network().snapshot_feed.port << std::endl;
        std::cout << "Incremental feed: " << config.network().incremental_feed_a.ip 
                  << ":" << config.network().incremental_feed_a.port << std::endl;
        
        // Create market data generator
        auto market_generator = std::make_shared<cme_mock::MarketDataGenerator>(
            order_book_manager, incremental_publisher);
        
        // Configure market behavior
        cme_mock::MarketConfig market_config;
        market_config.mode = cme_mock::MarketMode::Normal;
        market_config.volatility = 0.0001;
        market_config.orders_per_second = config.scenario().message_rate;
        market_config.trade_probability = 0.3;
        market_generator->set_config(market_config);
        
        std::cout << "\nServer is running. Press Ctrl+C to stop." << std::endl;
        std::cout << "Generating " << market_config.orders_per_second << " updates/second" << std::endl;
        
        // Timer tracking
        auto last_snapshot_time = std::chrono::steady_clock::now();
        auto last_update_time = std::chrono::steady_clock::now();
        auto last_stats_time = std::chrono::steady_clock::now();
        
        auto snapshot_interval = std::chrono::seconds(config.market_data().snapshot_interval_seconds);
        auto update_interval = std::chrono::milliseconds(1000 / market_config.orders_per_second);
        auto stats_interval = std::chrono::seconds(10);
        
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