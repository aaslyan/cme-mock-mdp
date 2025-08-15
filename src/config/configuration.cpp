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
    // Simple JSON parsing - parse actual instruments from JSON file
    try {
        // Default network config (would normally parse from JSON)
        network_.mode = "unicast";
        network_.incremental_feed_a = {"127.0.0.1", 14310};
        network_.incremental_feed_b = {"127.0.0.1", 14311};
        network_.snapshot_feed = {"127.0.0.1", 14320};
        network_.definition_feed = {"127.0.0.1", 14330};
        
        // Default market data config (would normally parse from JSON)
        market_data_.snapshot_interval_seconds = 30;
        market_data_.message_batch_size = 10;
        market_data_.max_book_depth = 10;
        market_data_.incremental_publish_interval_ms = 10;
        
        // Parse instruments from JSON content
        instruments_.clear();
        
        // Simple JSON parsing to extract instruments
        size_t instruments_start = json_content.find("\"instruments\":");
        if (instruments_start != std::string::npos) {
            size_t array_start = json_content.find("[", instruments_start);
            size_t array_end = json_content.find("]", array_start);
            
            if (array_start != std::string::npos && array_end != std::string::npos) {
                std::string instruments_section = json_content.substr(array_start + 1, array_end - array_start - 1);
                
                // Parse each instrument object
                size_t pos = 0;
                while (pos < instruments_section.length()) {
                    size_t obj_start = instruments_section.find("{", pos);
                    if (obj_start == std::string::npos) break;
                    
                    size_t obj_end = instruments_section.find("}", obj_start);
                    if (obj_end == std::string::npos) break;
                    
                    std::string instrument_obj = instruments_section.substr(obj_start, obj_end - obj_start + 1);
                    
                    // Extract instrument fields
                    InstrumentConfig instrument;
                    
                    // Parse symbol
                    size_t symbol_pos = instrument_obj.find("\"symbol\":");
                    if (symbol_pos != std::string::npos) {
                        size_t quote_start = instrument_obj.find("\"", symbol_pos + 9);
                        size_t quote_end = instrument_obj.find("\"", quote_start + 1);
                        if (quote_start != std::string::npos && quote_end != std::string::npos) {
                            instrument.symbol = instrument_obj.substr(quote_start + 1, quote_end - quote_start - 1);
                        }
                    }
                    
                    // Parse security_id
                    size_t id_pos = instrument_obj.find("\"security_id\":");
                    if (id_pos != std::string::npos) {
                        size_t num_start = id_pos + 14;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') num_end++;
                        if (num_end > num_start) {
                            instrument.security_id = std::stoi(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse name
                    size_t name_pos = instrument_obj.find("\"name\":");
                    if (name_pos != std::string::npos) {
                        size_t quote_start = instrument_obj.find("\"", name_pos + 7);
                        size_t quote_end = instrument_obj.find("\"", quote_start + 1);
                        if (quote_start != std::string::npos && quote_end != std::string::npos) {
                            instrument.name = instrument_obj.substr(quote_start + 1, quote_end - quote_start - 1);
                        }
                    }
                    
                    // Parse tick_size
                    size_t tick_pos = instrument_obj.find("\"tick_size\":");
                    if (tick_pos != std::string::npos) {
                        size_t num_start = tick_pos + 12;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.tick_size = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse multiplier
                    size_t mult_pos = instrument_obj.find("\"multiplier\":");
                    if (mult_pos != std::string::npos) {
                        size_t num_start = mult_pos + 13;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.multiplier = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse initial_price
                    size_t price_pos = instrument_obj.find("\"initial_price\":");
                    if (price_pos != std::string::npos) {
                        size_t num_start = price_pos + 16;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.initial_price = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse initial_spread
                    size_t spread_pos = instrument_obj.find("\"initial_spread\":");
                    if (spread_pos != std::string::npos) {
                        size_t num_start = spread_pos + 17;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.initial_spread = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse price_decimals
                    size_t dec_pos = instrument_obj.find("\"price_decimals\":");
                    if (dec_pos != std::string::npos) {
                        size_t num_start = dec_pos + 17;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') num_end++;
                        if (num_end > num_start) {
                            instrument.price_decimals = std::stoi(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse underlying
                    size_t underlying_pos = instrument_obj.find("\"underlying\":");
                    if (underlying_pos != std::string::npos) {
                        size_t quote_start = instrument_obj.find("\"", underlying_pos + 12);
                        size_t quote_end = instrument_obj.find("\"", quote_start + 1);
                        if (quote_start != std::string::npos && quote_end != std::string::npos) {
                            instrument.underlying = instrument_obj.substr(quote_start + 1, quote_end - quote_start - 1);
                        }
                    }
                    
                    // Parse exchange
                    size_t exchange_pos = instrument_obj.find("\"exchange\":");
                    if (exchange_pos != std::string::npos) {
                        size_t quote_start = instrument_obj.find("\"", exchange_pos + 10);
                        size_t quote_end = instrument_obj.find("\"", quote_start + 1);
                        if (quote_start != std::string::npos && quote_end != std::string::npos) {
                            instrument.exchange = instrument_obj.substr(quote_start + 1, quote_end - quote_start - 1);
                        }
                    }
                    
                    // Parse product_group
                    size_t group_pos = instrument_obj.find("\"product_group\":");
                    if (group_pos != std::string::npos) {
                        size_t quote_start = instrument_obj.find("\"", group_pos + 15);
                        size_t quote_end = instrument_obj.find("\"", quote_start + 1);
                        if (quote_start != std::string::npos && quote_end != std::string::npos) {
                            instrument.product_group = instrument_obj.substr(quote_start + 1, quote_end - quote_start - 1);
                        }
                    }
                    
                    // Parse contract_size
                    size_t csize_pos = instrument_obj.find("\"contract_size\":");
                    if (csize_pos != std::string::npos) {
                        size_t num_start = csize_pos + 16;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.contract_size = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse initial_margin
                    size_t margin_pos = instrument_obj.find("\"initial_margin\":");
                    if (margin_pos != std::string::npos) {
                        size_t num_start = margin_pos + 17;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.initial_margin = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse maintenance_margin
                    size_t maint_pos = instrument_obj.find("\"maintenance_margin\":");
                    if (maint_pos != std::string::npos) {
                        size_t num_start = maint_pos + 21;
                        while (num_start < instrument_obj.length() && (instrument_obj[num_start] == ' ' || instrument_obj[num_start] == ':')) num_start++;
                        size_t num_end = num_start;
                        while (num_end < instrument_obj.length() && 
                               ((instrument_obj[num_end] >= '0' && instrument_obj[num_end] <= '9') || instrument_obj[num_end] == '.')) num_end++;
                        if (num_end > num_start) {
                            instrument.maintenance_margin = std::stod(instrument_obj.substr(num_start, num_end - num_start));
                        }
                    }
                    
                    // Parse maturity_date
                    size_t maturity_pos = instrument_obj.find("\"maturity_date\":");
                    if (maturity_pos != std::string::npos) {
                        size_t quote_start = instrument_obj.find("\"", maturity_pos + 15);
                        size_t quote_end = instrument_obj.find("\"", quote_start + 1);
                        if (quote_start != std::string::npos && quote_end != std::string::npos) {
                            instrument.maturity_date = instrument_obj.substr(quote_start + 1, quote_end - quote_start - 1);
                        }
                    }
                    
                    // Add instrument to list if we got the required fields
                    if (!instrument.symbol.empty() && instrument.security_id > 0) {
                        instruments_.push_back(instrument);
                    }
                    
                    pos = obj_end + 1;
                }
            }
        }
        
        // If no instruments were parsed, add a single default
        if (instruments_.empty()) {
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
        }
        
        // Default scenario
        legacy_scenario_.type = "normal";
        legacy_scenario_.volatility = "medium";
        legacy_scenario_.message_rate = 100;
        legacy_scenario_.duration_minutes = 60;
        legacy_scenario_.price_drift = 0.0;
        legacy_scenario_.spread_multiplier = 1.0;
        legacy_scenario_.book_depth_target = 5;
        
        LOG_INFO("Configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse configuration: " + std::string(e.what()));
        return false;
    }
}

} // namespace cme_mock