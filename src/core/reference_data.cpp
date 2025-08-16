#include "core/reference_data.h"
#include "config/configuration.h"
#include "utils/logger.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace cme_mock {

// SessionManager implementation
SessionManager::SessionManager()
{
    // Initialize with default sessions
}

void SessionManager::add_session(const std::string& instrument_symbol, const TradingSession& session)
{
    instrument_sessions_[instrument_symbol].push_back(session);

    // Sort sessions by start time
    auto& sessions = instrument_sessions_[instrument_symbol];
    std::sort(sessions.begin(), sessions.end(),
        [](const TradingSession& a, const TradingSession& b) {
            return a.start_time < b.start_time;
        });
}

SessionState SessionManager::get_current_session_state(const std::string& instrument_symbol) const
{
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

TradingSession SessionManager::get_current_session(const std::string& instrument_symbol) const
{
    auto it = instrument_sessions_.find(instrument_symbol);
    if (it == instrument_sessions_.end()) {
        return TradingSession {};
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

    return TradingSession {};
}

std::vector<TradingSession> SessionManager::get_sessions(const std::string& instrument_symbol) const
{
    auto it = instrument_sessions_.find(instrument_symbol);
    if (it == instrument_sessions_.end()) {
        return {};
    }
    return it->second;
}

bool SessionManager::is_trading_hours(const std::string& instrument_symbol) const
{
    return get_current_session_state(instrument_symbol) == SessionState::Open;
}

std::chrono::system_clock::time_point SessionManager::get_next_session_start(const std::string& instrument_symbol) const
{
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

std::chrono::system_clock::time_point SessionManager::get_session_end(const std::string& instrument_symbol) const
{
    auto current_session = get_current_session(instrument_symbol);
    return current_session.end_time;
}

// ReferenceDataManager implementation
ReferenceDataManager& ReferenceDataManager::instance()
{
    static ReferenceDataManager instance;
    return instance;
}

bool ReferenceDataManager::initialize()
{
    Logger::instance().log(LogLevel::INFO, "Initializing Reference Data Manager");

    // Clear existing data
    instruments_.clear();
    symbol_to_id_.clear();

    // Load configuration and instruments
    auto& config = Configuration::instance();
    if (!config.load_from_file("config/server_config.json")) {
        Logger::instance().log(LogLevel::WARNING, "Failed to load config file, using default instruments");
        create_default_instruments();
    } else {
        Logger::instance().log(LogLevel::INFO, "Loading instruments from configuration");
        load_instruments(config.instruments());
    }

    Logger::instance().log(LogLevel::INFO, "Setting up trading sessions");

    // Setup default trading sessions
    setup_default_sessions();

    Logger::instance().log(LogLevel::INFO, "Reference Data Manager initialized with " + std::to_string(instruments_.size()) + " instruments");

    return true;
}

bool ReferenceDataManager::load_from_config(const std::string& /* config_file */)
{
    // For now, use the existing configuration system
    // This can be extended to load additional reference data from files
    return initialize();
}

bool ReferenceDataManager::load_instruments(const std::vector<InstrumentConfig>& configs)
{
    for (const auto& config : configs) {
        // Convert basic config to full instrument definition
        InstrumentDefinition instrument;

        // Basic info from config
        instrument.symbol = config.symbol;
        instrument.security_id = config.security_id;
        instrument.name = config.name;
        instrument.reference_price = config.initial_price;
        instrument.exchange = config.exchange;
        instrument.currency = "USD";

        // Contract specifications from config
        instrument.underlying_symbol = config.underlying;
        instrument.contract_size = config.contract_size;
        instrument.maturity_date = config.maturity_date;

        // Price format from config
        instrument.price_format.tick_size = config.tick_size;
        instrument.price_format.price_decimals = config.price_decimals;
        instrument.price_format.point_value = config.multiplier;
        instrument.price_format.tick_value = config.tick_size * config.multiplier;

        // Product group from config
        if (config.product_group == "Equity") {
            instrument.product_group = ProductGroup::Equity;
        } else if (config.product_group == "Energy") {
            instrument.product_group = ProductGroup::Energy;
        } else if (config.product_group == "Metals") {
            instrument.product_group = ProductGroup::Metals;
        } else if (config.product_group == "Interest") {
            instrument.product_group = ProductGroup::Interest;
        } else if (config.product_group == "FX") {
            instrument.product_group = ProductGroup::FX;
        } else if (config.product_group == "Agricultural") {
            instrument.product_group = ProductGroup::Agricultural;
        } else {
            instrument.product_group = ProductGroup::Equity; // Default to equity
        }

        // Risk parameters from config
        instrument.risk_params.initial_margin = config.initial_margin;
        instrument.risk_params.maintenance_margin = config.maintenance_margin;

        // Add to maps
        instruments_[instrument.security_id] = instrument;
        symbol_to_id_[instrument.symbol] = instrument.security_id;

        Logger::instance().log(LogLevel::INFO, "Loaded instrument: " + instrument.symbol + " (ID: " + std::to_string(instrument.security_id) + ")");
    }

    return true;
}

const InstrumentDefinition* ReferenceDataManager::get_instrument(uint32_t security_id) const
{
    auto it = instruments_.find(security_id);
    return (it != instruments_.end()) ? &it->second : nullptr;
}

const InstrumentDefinition* ReferenceDataManager::get_instrument(const std::string& symbol) const
{
    auto it = symbol_to_id_.find(symbol);
    if (it == symbol_to_id_.end()) {
        return nullptr;
    }

    return get_instrument(it->second);
}

std::vector<const InstrumentDefinition*> ReferenceDataManager::get_all_instruments() const
{
    std::vector<const InstrumentDefinition*> result;
    result.reserve(instruments_.size());

    for (const auto& pair : instruments_) {
        result.push_back(&pair.second);
    }

    return result;
}

std::vector<const InstrumentDefinition*> ReferenceDataManager::get_instruments_by_group(ProductGroup group) const
{
    std::vector<const InstrumentDefinition*> result;

    for (const auto& pair : instruments_) {
        if (pair.second.product_group == group) {
            result.push_back(&pair.second);
        }
    }

    return result;
}

bool ReferenceDataManager::add_instrument(const InstrumentDefinition& instrument)
{
    // Check for duplicates
    if (instruments_.find(instrument.security_id) != instruments_.end()) {
        Logger::instance().log(LogLevel::WARNING, "Instrument with ID " + std::to_string(instrument.security_id) + " already exists");
        return false;
    }

    if (symbol_to_id_.find(instrument.symbol) != symbol_to_id_.end()) {
        Logger::instance().log(LogLevel::WARNING, "Instrument with symbol " + instrument.symbol + " already exists");
        return false;
    }

    // Add instrument
    instruments_[instrument.security_id] = instrument;
    symbol_to_id_[instrument.symbol] = instrument.security_id;

    Logger::instance().log(LogLevel::INFO, "Added instrument: " + instrument.symbol + " (ID: " + std::to_string(instrument.security_id) + ")");

    return true;
}

bool ReferenceDataManager::update_instrument(uint32_t security_id, const InstrumentDefinition& instrument)
{
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

bool ReferenceDataManager::remove_instrument(uint32_t security_id)
{
    auto it = instruments_.find(security_id);
    if (it == instruments_.end()) {
        return false;
    }

    symbol_to_id_.erase(it->second.symbol);
    instruments_.erase(it);

    return true;
}

double ReferenceDataManager::round_price(uint32_t security_id, double price) const
{
    const auto* instrument = get_instrument(security_id);
    if (!instrument) {
        return price;
    }

    double tick_size = instrument->price_format.tick_size;
    return std::round(price / tick_size) * tick_size;
}

double ReferenceDataManager::get_tick_size(uint32_t security_id) const
{
    const auto* instrument = get_instrument(security_id);
    return instrument ? instrument->price_format.tick_size : 0.01;
}

double ReferenceDataManager::get_tick_value(uint32_t security_id) const
{
    const auto* instrument = get_instrument(security_id);
    return instrument ? instrument->price_format.tick_value : 1.0;
}

double ReferenceDataManager::calculate_tick_value(uint32_t security_id, double price_diff) const
{
    const auto* instrument = get_instrument(security_id);
    if (!instrument) {
        return price_diff;
    }

    double ticks = price_diff / instrument->price_format.tick_size;
    return ticks * instrument->price_format.tick_value;
}

void ReferenceDataManager::set_trading_status(uint32_t security_id, TradingStatus status)
{
    auto it = instruments_.find(security_id);
    if (it != instruments_.end()) {
        it->second.trading_status = status;

        std::string status_str;
        switch (status) {
        case TradingStatus::Trading:
            status_str = "Trading";
            break;
        case TradingStatus::Halted:
            status_str = "Halted";
            break;
        case TradingStatus::PreOpen:
            status_str = "PreOpen";
            break;
        case TradingStatus::NoCancel:
            status_str = "NoCancel";
            break;
        default:
            status_str = "Unknown";
            break;
        }

        Logger::instance().log(LogLevel::INFO, "Set trading status for " + it->second.symbol + " to " + status_str);
    }
}

TradingStatus ReferenceDataManager::get_trading_status(uint32_t security_id) const
{
    const auto* instrument = get_instrument(security_id);
    return instrument ? instrument->trading_status : TradingStatus::Unknown;
}

bool ReferenceDataManager::is_trading_allowed(uint32_t security_id) const
{
    TradingStatus status = get_trading_status(security_id);
    return status == TradingStatus::Trading || status == TradingStatus::NoCancel;
}

std::vector<std::string> ReferenceDataManager::get_supported_symbols() const
{
    std::vector<std::string> symbols;
    symbols.reserve(symbol_to_id_.size());

    for (const auto& pair : symbol_to_id_) {
        symbols.push_back(pair.first);
    }

    std::sort(symbols.begin(), symbols.end());
    return symbols;
}

void ReferenceDataManager::update_reference_price(uint32_t security_id, double price)
{
    auto it = instruments_.find(security_id);
    if (it != instruments_.end()) {
        it->second.reference_price = price;
        it->second.last_update_time = std::chrono::system_clock::now();
    }
}

void ReferenceDataManager::update_daily_stats(uint32_t security_id, double high, double low, uint64_t volume)
{
    auto it = instruments_.find(security_id);
    if (it != instruments_.end()) {
        it->second.high_price = high;
        it->second.low_price = low;
        it->second.volume = volume;
        it->second.last_update_time = std::chrono::system_clock::now();
    }
}

void ReferenceDataManager::create_default_instruments()
{
    // Create detailed instrument definitions for CME Channel 310 equity products
    auto es = create_es_future();
    instruments_[es.security_id] = es;
    symbol_to_id_[es.symbol] = es.security_id;

    auto nq = create_nq_future();
    instruments_[nq.security_id] = nq;
    symbol_to_id_[nq.symbol] = nq.security_id;

    auto ym = create_ym_future();
    instruments_[ym.security_id] = ym;
    symbol_to_id_[ym.symbol] = ym.security_id;

    auto rty = create_rty_future();
    instruments_[rty.security_id] = rty;
    symbol_to_id_[rty.symbol] = rty.security_id;

    auto emd = create_emd_future();
    instruments_[emd.security_id] = emd;
    symbol_to_id_[emd.symbol] = emd.security_id;

    auto mes = create_mes_future();
    instruments_[mes.security_id] = mes;
    symbol_to_id_[mes.symbol] = mes.security_id;

    auto mnq = create_mnq_future();
    instruments_[mnq.security_id] = mnq;
    symbol_to_id_[mnq.symbol] = mnq.security_id;

    auto mym = create_mym_future();
    instruments_[mym.security_id] = mym;
    symbol_to_id_[mym.symbol] = mym.security_id;

    auto m2k = create_m2k_future();
    instruments_[m2k.security_id] = m2k;
    symbol_to_id_[m2k.symbol] = m2k.security_id;

    auto vx = create_vx_future();
    instruments_[vx.security_id] = vx;
    symbol_to_id_[vx.symbol] = vx.security_id;
}

InstrumentDefinition ReferenceDataManager::create_es_future() const
{
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
    es.contract_size = 50.0; // $50 per index point
    es.maturity_date = "2024-12-20";
    es.last_trading_date = "2024-12-19";

    // Price formatting
    es.price_format.tick_size = 0.25;
    es.price_format.price_decimals = 2;
    es.price_format.point_value = 50.0;
    es.price_format.tick_value = 12.50; // 0.25 * 50
    es.price_format.price_precision = 4;

    // Risk parameters
    es.risk_params.initial_margin = 13200.0;
    es.risk_params.maintenance_margin = 12000.0;
    es.risk_params.max_order_value = 100000000.0; // $100M

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

InstrumentDefinition ReferenceDataManager::create_nq_future() const
{
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
    nq.contract_size = 20.0; // $20 per index point
    nq.maturity_date = "2024-12-20";

    nq.price_format.tick_size = 0.25;
    nq.price_format.price_decimals = 2;
    nq.price_format.point_value = 20.0;
    nq.price_format.tick_value = 5.00; // 0.25 * 20

    nq.risk_params.initial_margin = 17600.0;
    nq.risk_params.maintenance_margin = 16000.0;

    nq.reference_price = 15500.00;
    nq.opening_price = 15485.25;
    nq.previous_settle = 15492.50;

    nq.creation_time = std::chrono::system_clock::now();

    return nq;
}

InstrumentDefinition ReferenceDataManager::create_ym_future() const
{
    InstrumentDefinition ym;

    ym.symbol = "YM";
    ym.security_id = 3;
    ym.name = "E-mini Dow Jones Industrial Average";
    ym.description = "E-mini Dow Jones Industrial Average Future";

    ym.instrument_type = InstrumentType::Future;
    ym.product_group = ProductGroup::Equity;
    ym.exchange = "CME";
    ym.currency = "USD";

    ym.underlying_symbol = "DJIA";
    ym.contract_size = 5.0; // $5 per index point
    ym.maturity_date = "2024-12-20";
    ym.last_trading_date = "2024-12-19";

    ym.price_format.tick_size = 1.0;
    ym.price_format.price_decimals = 0;
    ym.price_format.point_value = 5.0;
    ym.price_format.tick_value = 5.00; // 1.0 * 5
    ym.price_format.price_precision = 5;

    ym.risk_params.initial_margin = 9800.0;
    ym.risk_params.maintenance_margin = 8900.0;
    ym.risk_params.max_order_value = 50000000.0;

    ym.min_quote_size = 1.0;
    ym.round_lot_size = 1.0;
    ym.max_book_depth = 10;

    ym.reference_price = 34000.00;
    ym.opening_price = 33985.00;
    ym.previous_settle = 33990.00;

    ym.creation_time = std::chrono::system_clock::now();

    return ym;
}

InstrumentDefinition ReferenceDataManager::create_rty_future() const
{
    InstrumentDefinition rty;

    rty.symbol = "RTY";
    rty.security_id = 4;
    rty.name = "E-mini Russell 2000";
    rty.description = "E-mini Russell 2000 Index Future";

    rty.instrument_type = InstrumentType::Future;
    rty.product_group = ProductGroup::Equity;
    rty.exchange = "CME";
    rty.currency = "USD";

    rty.underlying_symbol = "RUT";
    rty.contract_size = 50.0; // $50 per index point
    rty.maturity_date = "2024-12-20";

    rty.price_format.tick_size = 0.10;
    rty.price_format.price_decimals = 1;
    rty.price_format.point_value = 50.0;
    rty.price_format.tick_value = 5.00; // 0.10 * 50

    rty.risk_params.initial_margin = 7200.0;
    rty.risk_params.maintenance_margin = 6500.0;

    rty.reference_price = 1900.00;
    rty.opening_price = 1895.50;
    rty.previous_settle = 1898.20;

    rty.creation_time = std::chrono::system_clock::now();

    return rty;
}

InstrumentDefinition ReferenceDataManager::create_emd_future() const
{
    InstrumentDefinition emd;

    emd.symbol = "EMD";
    emd.security_id = 5;
    emd.name = "E-mini S&P MidCap 400";
    emd.description = "E-mini S&P MidCap 400 Index Future";

    emd.instrument_type = InstrumentType::Future;
    emd.product_group = ProductGroup::Equity;
    emd.exchange = "CME";
    emd.currency = "USD";

    emd.underlying_symbol = "MID";
    emd.contract_size = 100.0; // $100 per index point
    emd.maturity_date = "2024-12-20";

    emd.price_format.tick_size = 0.10;
    emd.price_format.price_decimals = 1;
    emd.price_format.point_value = 100.0;
    emd.price_format.tick_value = 10.00; // 0.10 * 100

    emd.risk_params.initial_margin = 12000.0;
    emd.risk_params.maintenance_margin = 11000.0;

    emd.reference_price = 2700.00;
    emd.opening_price = 2695.50;
    emd.previous_settle = 2698.80;

    emd.creation_time = std::chrono::system_clock::now();

    return emd;
}

InstrumentDefinition ReferenceDataManager::create_mes_future() const
{
    InstrumentDefinition mes;

    mes.symbol = "MES";
    mes.security_id = 6;
    mes.name = "Micro E-mini S&P 500";
    mes.description = "Micro E-mini S&P 500 Future";

    mes.instrument_type = InstrumentType::Future;
    mes.product_group = ProductGroup::Equity;
    mes.exchange = "CME";
    mes.currency = "USD";

    mes.underlying_symbol = "SPX";
    mes.contract_size = 5.0; // $5 per index point (1/10th of ES)
    mes.maturity_date = "2024-12-20";

    mes.price_format.tick_size = 0.25;
    mes.price_format.price_decimals = 2;
    mes.price_format.point_value = 5.0;
    mes.price_format.tick_value = 1.25; // 0.25 * 5

    mes.risk_params.initial_margin = 1380.0; // ~1/10th of ES
    mes.risk_params.maintenance_margin = 1200.0;

    mes.reference_price = 4500.00;
    mes.opening_price = 4498.50;
    mes.previous_settle = 4497.75;

    mes.creation_time = std::chrono::system_clock::now();

    return mes;
}

InstrumentDefinition ReferenceDataManager::create_mnq_future() const
{
    InstrumentDefinition mnq;

    mnq.symbol = "MNQ";
    mnq.security_id = 7;
    mnq.name = "Micro E-mini NASDAQ-100";
    mnq.description = "Micro E-mini NASDAQ-100 Future";

    mnq.instrument_type = InstrumentType::Future;
    mnq.product_group = ProductGroup::Equity;
    mnq.exchange = "CME";
    mnq.currency = "USD";

    mnq.underlying_symbol = "NDX";
    mnq.contract_size = 2.0; // $2 per index point (1/10th of NQ)
    mnq.maturity_date = "2024-12-20";

    mnq.price_format.tick_size = 0.25;
    mnq.price_format.price_decimals = 2;
    mnq.price_format.point_value = 2.0;
    mnq.price_format.tick_value = 0.50; // 0.25 * 2

    mnq.risk_params.initial_margin = 1760.0; // ~1/10th of NQ
    mnq.risk_params.maintenance_margin = 1600.0;

    mnq.reference_price = 15500.00;
    mnq.opening_price = 15485.25;
    mnq.previous_settle = 15492.50;

    mnq.creation_time = std::chrono::system_clock::now();

    return mnq;
}

InstrumentDefinition ReferenceDataManager::create_mym_future() const
{
    InstrumentDefinition mym;

    mym.symbol = "MYM";
    mym.security_id = 8;
    mym.name = "Micro E-mini Dow Jones Industrial Average";
    mym.description = "Micro E-mini Dow Jones Industrial Average Future";

    mym.instrument_type = InstrumentType::Future;
    mym.product_group = ProductGroup::Equity;
    mym.exchange = "CME";
    mym.currency = "USD";

    mym.underlying_symbol = "DJIA";
    mym.contract_size = 0.5; // $0.50 per index point (1/10th of YM)
    mym.maturity_date = "2024-12-20";

    mym.price_format.tick_size = 1.0;
    mym.price_format.price_decimals = 0;
    mym.price_format.point_value = 0.5;
    mym.price_format.tick_value = 0.50; // 1.0 * 0.5

    mym.risk_params.initial_margin = 980.0; // ~1/10th of YM
    mym.risk_params.maintenance_margin = 890.0;

    mym.reference_price = 34000.00;
    mym.opening_price = 33985.00;
    mym.previous_settle = 33990.00;

    mym.creation_time = std::chrono::system_clock::now();

    return mym;
}

InstrumentDefinition ReferenceDataManager::create_m2k_future() const
{
    InstrumentDefinition m2k;

    m2k.symbol = "M2K";
    m2k.security_id = 9;
    m2k.name = "Micro E-mini Russell 2000";
    m2k.description = "Micro E-mini Russell 2000 Index Future";

    m2k.instrument_type = InstrumentType::Future;
    m2k.product_group = ProductGroup::Equity;
    m2k.exchange = "CME";
    m2k.currency = "USD";

    m2k.underlying_symbol = "RUT";
    m2k.contract_size = 5.0; // $5 per index point (1/10th of RTY)
    m2k.maturity_date = "2024-12-20";

    m2k.price_format.tick_size = 0.10;
    m2k.price_format.price_decimals = 1;
    m2k.price_format.point_value = 5.0;
    m2k.price_format.tick_value = 0.50; // 0.10 * 5

    m2k.risk_params.initial_margin = 720.0; // ~1/10th of RTY
    m2k.risk_params.maintenance_margin = 650.0;

    m2k.reference_price = 1900.00;
    m2k.opening_price = 1895.50;
    m2k.previous_settle = 1898.20;

    m2k.creation_time = std::chrono::system_clock::now();

    return m2k;
}

InstrumentDefinition ReferenceDataManager::create_vx_future() const
{
    InstrumentDefinition vx;

    vx.symbol = "VX";
    vx.security_id = 10;
    vx.name = "CBOE Volatility Index";
    vx.description = "VIX Futures Contract";

    vx.instrument_type = InstrumentType::Future;
    vx.product_group = ProductGroup::Equity; // Volatility related to equity
    vx.exchange = "CME";
    vx.currency = "USD";

    vx.underlying_symbol = "VIX";
    vx.contract_size = 1000.0; // $1000 per index point
    vx.maturity_date = "2024-12-18"; // VIX expires on Wednesdays

    vx.price_format.tick_size = 0.05;
    vx.price_format.price_decimals = 2;
    vx.price_format.point_value = 1000.0;
    vx.price_format.tick_value = 50.00; // 0.05 * 1000

    vx.risk_params.initial_margin = 4000.0;
    vx.risk_params.maintenance_margin = 3600.0;

    vx.reference_price = 18.50;
    vx.opening_price = 18.25;
    vx.previous_settle = 18.40;

    vx.creation_time = std::chrono::system_clock::now();

    return vx;
}

void ReferenceDataManager::setup_default_sessions()
{
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

TradingSession ReferenceDataManager::create_rth_session() const
{
    TradingSession rth;
    rth.session_id = "RTH";
    rth.name = "Regular Trading Hours";
    rth.is_primary = true;
    rth.max_order_size = 1000000;

    // 9:30 AM - 4:15 PM ET (simplified - using current time base)
    auto now = std::chrono::system_clock::now();
    rth.start_time = now; // Simplified for demo
    rth.end_time = now + std::chrono::hours(24); // Always open for demo

    return rth;
}

TradingSession ReferenceDataManager::create_eth_session() const
{
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