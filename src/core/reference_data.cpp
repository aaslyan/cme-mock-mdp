#include "core/reference_data.h"
#include "config/configuration.h"
#include "utils/logger.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace cme_mock {

// SessionManager implementation
SessionManager::SessionManager() {
    // Initialize with default sessions
}

void SessionManager::add_session(const std::string& instrument_symbol, const TradingSession& session) {
    instrument_sessions_[instrument_symbol].push_back(session);
    
    // Sort sessions by start time
    auto& sessions = instrument_sessions_[instrument_symbol];
    std::sort(sessions.begin(), sessions.end(), 
              [](const TradingSession& a, const TradingSession& b) {
                  return a.start_time < b.start_time;
              });
}

SessionState SessionManager::get_current_session_state(const std::string& instrument_symbol) const {
    auto it = instrument_sessions_.find(instrument_symbol);
    if (it == instrument_sessions_.end()) {
        return SessionState::Close;
    }
    
    auto now = std::chrono::system_clock::now();
    
    for (const auto& session : it->second) {
        if (now >= session.start_time && now <= session.end_time) {
            return SessionState::Open;
        }
        if (now < session.start_time) {
            return SessionState::PreOpen;
        }
    }
    
    return SessionState::Close;
}

TradingSession SessionManager::get_current_session(const std::string& instrument_symbol) const {
    auto it = instrument_sessions_.find(instrument_symbol);
    if (it == instrument_sessions_.end()) {
        return TradingSession{};
    }
    
    auto now = std::chrono::system_clock::now();
    
    for (const auto& session : it->second) {
        if (now >= session.start_time && now <= session.end_time) {
            return session;
        }
    }
    
    // Return primary session if no current session
    for (const auto& session : it->second) {
        if (session.is_primary) {
            return session;
        }
    }
    
    // Return first session as fallback
    if (!it->second.empty()) {
        return it->second[0];
    }
    
    return TradingSession{};
}

std::vector<TradingSession> SessionManager::get_sessions(const std::string& instrument_symbol) const {
    auto it = instrument_sessions_.find(instrument_symbol);
    if (it == instrument_sessions_.end()) {
        return {};
    }
    return it->second;
}

bool SessionManager::is_trading_hours(const std::string& instrument_symbol) const {
    return get_current_session_state(instrument_symbol) == SessionState::Open;
}

std::chrono::system_clock::time_point SessionManager::get_next_session_start(const std::string& instrument_symbol) const {
    auto it = instrument_sessions_.find(instrument_symbol);
    if (it == instrument_sessions_.end()) {
        return std::chrono::system_clock::now();
    }
    
    auto now = std::chrono::system_clock::now();
    
    for (const auto& session : it->second) {
        if (session.start_time > now) {
            return session.start_time;
        }
    }
    
    // Return first session start time (assuming daily cycle)
    if (!it->second.empty()) {
        return it->second[0].start_time;
    }
    
    return now;
}

std::chrono::system_clock::time_point SessionManager::get_session_end(const std::string& instrument_symbol) const {
    auto current_session = get_current_session(instrument_symbol);
    return current_session.end_time;
}

// ReferenceDataManager implementation
ReferenceDataManager& ReferenceDataManager::instance() {
    static ReferenceDataManager instance;
    return instance;
}

bool ReferenceDataManager::initialize() {
    Logger::instance().log(LogLevel::INFO, "Initializing Reference Data Manager");
    
    // Clear existing data
    instruments_.clear();
    symbol_to_id_.clear();
    
    Logger::instance().log(LogLevel::INFO, "Creating default instruments");
    
    // Create default CME instruments
    create_default_instruments();
    
    Logger::instance().log(LogLevel::INFO, "Setting up trading sessions");
    
    // Setup default trading sessions
    setup_default_sessions();
    
    Logger::instance().log(LogLevel::INFO, "Reference Data Manager initialized with " + 
                          std::to_string(instruments_.size()) + " instruments");
    
    return true;
}

bool ReferenceDataManager::load_from_config(const std::string& config_file) {
    // For now, use the existing configuration system
    // This can be extended to load additional reference data from files
    return initialize();
}

bool ReferenceDataManager::load_instruments(const std::vector<InstrumentConfig>& configs) {
    for (const auto& config : configs) {
        // Convert basic config to full instrument definition
        InstrumentDefinition instrument;
        
        // Basic info from config
        instrument.symbol = config.symbol;
        instrument.security_id = config.security_id;
        instrument.name = config.name;
        instrument.reference_price = config.initial_price;
        
        // Price format
        instrument.price_format.tick_size = config.tick_size;
        instrument.price_format.price_decimals = config.price_decimals;
        instrument.price_format.point_value = config.multiplier;
        instrument.price_format.tick_value = config.tick_size * config.multiplier;
        
        // Set product group based on symbol
        if (config.symbol == "ES" || config.symbol == "NQ") {
            instrument.product_group = ProductGroup::Equity;
        } else if (config.symbol == "CL" || config.symbol == "NG") {
            instrument.product_group = ProductGroup::Energy;
        } else if (config.symbol == "GC" || config.symbol == "SI") {
            instrument.product_group = ProductGroup::Metals;
        }
        
        // Add to maps
        instruments_[instrument.security_id] = instrument;
        symbol_to_id_[instrument.symbol] = instrument.security_id;
        
        Logger::instance().log(LogLevel::INFO, "Loaded instrument: " + instrument.symbol + 
                              " (ID: " + std::to_string(instrument.security_id) + ")");
    }
    
    return true;
}

const InstrumentDefinition* ReferenceDataManager::get_instrument(uint32_t security_id) const {
    auto it = instruments_.find(security_id);
    return (it != instruments_.end()) ? &it->second : nullptr;
}

const InstrumentDefinition* ReferenceDataManager::get_instrument(const std::string& symbol) const {
    auto it = symbol_to_id_.find(symbol);
    if (it == symbol_to_id_.end()) {
        return nullptr;
    }
    
    return get_instrument(it->second);
}

std::vector<const InstrumentDefinition*> ReferenceDataManager::get_all_instruments() const {
    std::vector<const InstrumentDefinition*> result;
    result.reserve(instruments_.size());
    
    for (const auto& pair : instruments_) {
        result.push_back(&pair.second);
    }
    
    return result;
}

std::vector<const InstrumentDefinition*> ReferenceDataManager::get_instruments_by_group(ProductGroup group) const {
    std::vector<const InstrumentDefinition*> result;
    
    for (const auto& pair : instruments_) {
        if (pair.second.product_group == group) {
            result.push_back(&pair.second);
        }
    }
    
    return result;
}

bool ReferenceDataManager::add_instrument(const InstrumentDefinition& instrument) {
    // Check for duplicates
    if (instruments_.find(instrument.security_id) != instruments_.end()) {
        Logger::instance().log(LogLevel::WARNING, "Instrument with ID " + 
                              std::to_string(instrument.security_id) + " already exists");
        return false;
    }
    
    if (symbol_to_id_.find(instrument.symbol) != symbol_to_id_.end()) {
        Logger::instance().log(LogLevel::WARNING, "Instrument with symbol " + 
                              instrument.symbol + " already exists");
        return false;
    }
    
    // Add instrument
    instruments_[instrument.security_id] = instrument;
    symbol_to_id_[instrument.symbol] = instrument.security_id;
    
    Logger::instance().log(LogLevel::INFO, "Added instrument: " + instrument.symbol + 
                          " (ID: " + std::to_string(instrument.security_id) + ")");
    
    return true;
}

bool ReferenceDataManager::update_instrument(uint32_t security_id, const InstrumentDefinition& instrument) {
    auto it = instruments_.find(security_id);
    if (it == instruments_.end()) {
        return false;
    }
    
    // Update symbol mapping if symbol changed
    if (it->second.symbol != instrument.symbol) {
        symbol_to_id_.erase(it->second.symbol);
        symbol_to_id_[instrument.symbol] = security_id;
    }
    
    it->second = instrument;
    return true;
}

bool ReferenceDataManager::remove_instrument(uint32_t security_id) {
    auto it = instruments_.find(security_id);
    if (it == instruments_.end()) {
        return false;
    }
    
    symbol_to_id_.erase(it->second.symbol);
    instruments_.erase(it);
    
    return true;
}

double ReferenceDataManager::round_price(uint32_t security_id, double price) const {
    const auto* instrument = get_instrument(security_id);
    if (!instrument) {
        return price;
    }
    
    double tick_size = instrument->price_format.tick_size;
    return std::round(price / tick_size) * tick_size;
}

double ReferenceDataManager::get_tick_size(uint32_t security_id) const {
    const auto* instrument = get_instrument(security_id);
    return instrument ? instrument->price_format.tick_size : 0.01;
}

double ReferenceDataManager::get_tick_value(uint32_t security_id) const {
    const auto* instrument = get_instrument(security_id);
    return instrument ? instrument->price_format.tick_value : 1.0;
}

double ReferenceDataManager::calculate_tick_value(uint32_t security_id, double price_diff) const {
    const auto* instrument = get_instrument(security_id);
    if (!instrument) {
        return price_diff;
    }
    
    double ticks = price_diff / instrument->price_format.tick_size;
    return ticks * instrument->price_format.tick_value;
}

void ReferenceDataManager::set_trading_status(uint32_t security_id, TradingStatus status) {
    auto it = instruments_.find(security_id);
    if (it != instruments_.end()) {
        it->second.trading_status = status;
        
        std::string status_str;
        switch (status) {
            case TradingStatus::Trading: status_str = "Trading"; break;
            case TradingStatus::Halted: status_str = "Halted"; break;
            case TradingStatus::PreOpen: status_str = "PreOpen"; break;
            case TradingStatus::NoCancel: status_str = "NoCancel"; break;
            default: status_str = "Unknown"; break;
        }
        
        Logger::instance().log(LogLevel::INFO, "Set trading status for " + 
                              it->second.symbol + " to " + status_str);
    }
}

TradingStatus ReferenceDataManager::get_trading_status(uint32_t security_id) const {
    const auto* instrument = get_instrument(security_id);
    return instrument ? instrument->trading_status : TradingStatus::Unknown;
}

bool ReferenceDataManager::is_trading_allowed(uint32_t security_id) const {
    TradingStatus status = get_trading_status(security_id);
    return status == TradingStatus::Trading || status == TradingStatus::NoCancel;
}

std::vector<std::string> ReferenceDataManager::get_supported_symbols() const {
    std::vector<std::string> symbols;
    symbols.reserve(symbol_to_id_.size());
    
    for (const auto& pair : symbol_to_id_) {
        symbols.push_back(pair.first);
    }
    
    std::sort(symbols.begin(), symbols.end());
    return symbols;
}

void ReferenceDataManager::update_reference_price(uint32_t security_id, double price) {
    auto it = instruments_.find(security_id);
    if (it != instruments_.end()) {
        it->second.reference_price = price;
        it->second.last_update_time = std::chrono::system_clock::now();
    }
}

void ReferenceDataManager::update_daily_stats(uint32_t security_id, double high, double low, uint64_t volume) {
    auto it = instruments_.find(security_id);
    if (it != instruments_.end()) {
        it->second.high_price = high;
        it->second.low_price = low;
        it->second.volume = volume;
        it->second.last_update_time = std::chrono::system_clock::now();
    }
}

void ReferenceDataManager::create_default_instruments() {
    // Create detailed instrument definitions (called with lock already held)
    auto es = create_es_future();
    instruments_[es.security_id] = es;
    symbol_to_id_[es.symbol] = es.security_id;
    
    auto nq = create_nq_future();
    instruments_[nq.security_id] = nq;
    symbol_to_id_[nq.symbol] = nq.security_id;
    
    auto cl = create_cl_future();
    instruments_[cl.security_id] = cl;
    symbol_to_id_[cl.symbol] = cl.security_id;
    
    auto gc = create_gc_future();
    instruments_[gc.security_id] = gc;
    symbol_to_id_[gc.symbol] = gc.security_id;
}

InstrumentDefinition ReferenceDataManager::create_es_future() const {
    InstrumentDefinition es;
    
    // Basic identification
    es.symbol = "ES";
    es.security_id = 1;
    es.name = "E-mini S&P 500";
    es.description = "E-mini S&P 500 Future";
    
    // Classification
    es.instrument_type = InstrumentType::Future;
    es.product_group = ProductGroup::Equity;
    es.exchange = "CME";
    es.currency = "USD";
    
    // Contract specifications
    es.underlying_symbol = "SPX";
    es.contract_size = 50.0;  // $50 per index point
    es.maturity_date = "2024-12-20";
    es.last_trading_date = "2024-12-19";
    
    // Price formatting
    es.price_format.tick_size = 0.25;
    es.price_format.price_decimals = 2;
    es.price_format.point_value = 50.0;
    es.price_format.tick_value = 12.50;  // 0.25 * 50
    es.price_format.price_precision = 4;
    
    // Risk parameters
    es.risk_params.initial_margin = 13200.0;
    es.risk_params.maintenance_margin = 12000.0;
    es.risk_params.max_order_value = 100000000.0;  // $100M
    
    // Trading parameters
    es.min_quote_size = 1.0;
    es.round_lot_size = 1.0;
    es.max_book_depth = 10;
    
    // Reference prices
    es.reference_price = 4500.00;
    es.opening_price = 4498.50;
    es.previous_settle = 4497.75;
    
    es.creation_time = std::chrono::system_clock::now();
    
    return es;
}

InstrumentDefinition ReferenceDataManager::create_nq_future() const {
    InstrumentDefinition nq;
    
    nq.symbol = "NQ";
    nq.security_id = 2;
    nq.name = "E-mini NASDAQ-100";
    nq.description = "E-mini NASDAQ-100 Future";
    
    nq.instrument_type = InstrumentType::Future;
    nq.product_group = ProductGroup::Equity;
    nq.exchange = "CME";
    nq.currency = "USD";
    
    nq.underlying_symbol = "NDX";
    nq.contract_size = 20.0;  // $20 per index point
    nq.maturity_date = "2024-12-20";
    
    nq.price_format.tick_size = 0.25;
    nq.price_format.price_decimals = 2;
    nq.price_format.point_value = 20.0;
    nq.price_format.tick_value = 5.00;  // 0.25 * 20
    
    nq.risk_params.initial_margin = 17600.0;
    nq.risk_params.maintenance_margin = 16000.0;
    
    nq.reference_price = 15500.00;
    nq.opening_price = 15485.25;
    nq.previous_settle = 15492.50;
    
    nq.creation_time = std::chrono::system_clock::now();
    
    return nq;
}

InstrumentDefinition ReferenceDataManager::create_cl_future() const {
    InstrumentDefinition cl;
    
    cl.symbol = "CL";
    cl.security_id = 3;
    cl.name = "Crude Oil";
    cl.description = "Light Sweet Crude Oil Future";
    
    cl.instrument_type = InstrumentType::Future;
    cl.product_group = ProductGroup::Energy;
    cl.exchange = "NYMEX";
    cl.currency = "USD";
    
    cl.underlying_symbol = "WTI";
    cl.contract_size = 1000.0;  // 1,000 barrels
    cl.maturity_date = "2024-12-20";
    
    cl.price_format.tick_size = 0.01;
    cl.price_format.price_decimals = 2;
    cl.price_format.point_value = 1000.0;
    cl.price_format.tick_value = 10.00;  // 0.01 * 1000
    
    cl.risk_params.initial_margin = 6050.0;
    cl.risk_params.maintenance_margin = 5500.0;
    
    cl.reference_price = 75.50;
    cl.opening_price = 75.25;
    cl.previous_settle = 75.18;
    
    cl.creation_time = std::chrono::system_clock::now();
    
    return cl;
}

InstrumentDefinition ReferenceDataManager::create_gc_future() const {
    InstrumentDefinition gc;
    
    gc.symbol = "GC";
    gc.security_id = 4;
    gc.name = "Gold";
    gc.description = "Gold Future";
    
    gc.instrument_type = InstrumentType::Future;
    gc.product_group = ProductGroup::Metals;
    gc.exchange = "COMEX";
    gc.currency = "USD";
    
    gc.underlying_symbol = "XAU";
    gc.contract_size = 100.0;  // 100 troy ounces
    gc.maturity_date = "2024-12-27";
    
    gc.price_format.tick_size = 0.10;
    gc.price_format.price_decimals = 1;
    gc.price_format.point_value = 100.0;
    gc.price_format.tick_value = 10.00;  // 0.10 * 100
    
    gc.risk_params.initial_margin = 6050.0;
    gc.risk_params.maintenance_margin = 5500.0;
    
    gc.reference_price = 1950.0;
    gc.opening_price = 1948.5;
    gc.previous_settle = 1952.3;
    
    gc.creation_time = std::chrono::system_clock::now();
    
    return gc;
}

void ReferenceDataManager::setup_default_sessions() {
    // Setup trading sessions for each instrument (called with lock already held)
    std::vector<std::string> symbols;
    for (const auto& pair : symbol_to_id_) {
        symbols.push_back(pair.first);
    }
    
    for (const auto& symbol : symbols) {
        // Regular Trading Hours (RTH)
        session_manager_.add_session(symbol, create_rth_session());
        
        // Electronic Trading Hours (ETH) 
        session_manager_.add_session(symbol, create_eth_session());
    }
}

TradingSession ReferenceDataManager::create_rth_session() const {
    TradingSession rth;
    rth.session_id = "RTH";
    rth.name = "Regular Trading Hours";
    rth.is_primary = true;
    rth.max_order_size = 1000000;
    
    // 9:30 AM - 4:15 PM ET (simplified - using current time base)
    auto now = std::chrono::system_clock::now();
    rth.start_time = now;  // Simplified for demo
    rth.end_time = now + std::chrono::hours(24);  // Always open for demo
    
    return rth;
}

TradingSession ReferenceDataManager::create_eth_session() const {
    TradingSession eth;
    eth.session_id = "ETH";
    eth.name = "Electronic Trading Hours";
    eth.is_primary = false;
    eth.max_order_size = 500000;
    
    // Extended hours (simplified)
    auto now = std::chrono::system_clock::now();
    eth.start_time = now;
    eth.end_time = now + std::chrono::hours(24);
    
    return eth;
}

} // namespace cme_mock