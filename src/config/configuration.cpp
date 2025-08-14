#include "config/configuration.h"
#include "utils/logger.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace cme_mock {

Configuration& Configuration::instance() {
    static Configuration instance;
    return instance;
}

bool Configuration::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open configuration file: " + filename);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return load_from_json(buffer.str());
}

bool Configuration::load_from_json(const std::string& json_content) {
    // Simple JSON parsing - in production, use a proper JSON library
    try {
        // For now, use default values
        // A real implementation would parse the JSON
        
        // Default network config
        network_.mode = "unicast";
        network_.incremental_feed_a = {"127.0.0.1", 14310};
        network_.incremental_feed_b = {"127.0.0.1", 14311};
        network_.snapshot_feed = {"127.0.0.1", 14320};
        network_.definition_feed = {"127.0.0.1", 14330};
        
        // Default market data config
        market_data_.snapshot_interval_seconds = 30;
        market_data_.message_batch_size = 10;
        market_data_.max_book_depth = 10;
        market_data_.incremental_publish_interval_ms = 10;
        
        // Default instruments
        instruments_.clear();
        
        InstrumentConfig es;
        es.symbol = "ES";
        es.security_id = 1;
        es.name = "E-mini S&P 500";
        es.tick_size = 0.25;
        es.multiplier = 50;
        es.initial_price = 4500.00;
        es.initial_spread = 0.25;
        es.price_decimals = 2;
        instruments_.push_back(es);
        
        InstrumentConfig nq;
        nq.symbol = "NQ";
        nq.security_id = 2;
        nq.name = "E-mini NASDAQ-100";
        nq.tick_size = 0.25;
        nq.multiplier = 20;
        nq.initial_price = 15500.00;
        nq.initial_spread = 0.25;
        nq.price_decimals = 2;
        instruments_.push_back(nq);
        
        InstrumentConfig cl;
        cl.symbol = "CL";
        cl.security_id = 3;
        cl.name = "Crude Oil";
        cl.tick_size = 0.01;
        cl.multiplier = 1000;
        cl.initial_price = 75.50;
        cl.initial_spread = 0.01;
        cl.price_decimals = 2;
        instruments_.push_back(cl);
        
        InstrumentConfig gc;
        gc.symbol = "GC";
        gc.security_id = 4;
        gc.name = "Gold";
        gc.tick_size = 0.10;
        gc.multiplier = 100;
        gc.initial_price = 1950.00;
        gc.initial_spread = 0.10;
        gc.price_decimals = 1;
        instruments_.push_back(gc);
        
        // Default scenario
        scenario_.type = ScenarioType::NormalMarket;
        scenario_.volatility = "medium";
        scenario_.message_rate = 100;
        scenario_.duration_minutes = 60;
        scenario_.price_drift = 0.0;
        scenario_.spread_multiplier = 1.0;
        scenario_.book_depth_target = 5;
        
        LOG_INFO("Configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse configuration: " + std::string(e.what()));
        return false;
    }
}

} // namespace cme_mock