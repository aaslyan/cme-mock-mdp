#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace cme_mock {

// Market session state
enum class SessionState {
    PreOpen,
    Open,
    Pause,
    Close,
    PostClose
};

// Trading status for instruments
enum class TradingStatus {
    Trading,
    Halted,
    PreOpen,
    NoCancel,
    Reserved,
    Unknown
};

// Instrument types following CME classification
enum class InstrumentType {
    Future,
    Option,
    Spread,
    Index,
    Cash
};

// Product groups from CME
enum class ProductGroup {
    Equity, // ES, NQ, etc.
    Interest, // ZN, ZB, etc.
    FX, // EUR, GBP, etc.
    Energy, // CL, NG, etc.
    Metals, // GC, SI, etc.
    Agricultural // ZC, ZS, etc.
};

// Market data entry types
enum class EntryType {
    Bid = '0',
    Ask = '1',
    Trade = '2',
    OpenPrice = '4',
    SettlementPrice = '6',
    TradingSessionHighPrice = '7',
    TradingSessionLowPrice = '8',
    TradingSessionVWAPPrice = '9'
};

// Price formatting information
struct PriceFormat {
    uint8_t price_decimals = 2; // Number of decimal places
    double tick_size = 0.25; // Minimum price increment
    double tick_value = 12.50; // Dollar value per tick
    double point_value = 50.0; // Dollar value per point
    uint8_t price_precision = 4; // Total significant digits
};

// Trading session definition
struct TradingSession {
    std::string session_id; // "ETH", "RTH", etc.
    std::string name; // "Electronic Trading Hours"
    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point end_time;
    bool is_primary = false; // Primary trading session
    uint32_t max_order_size = 1000000; // Maximum order quantity
};

// Risk and margin parameters
struct RiskParameters {
    double initial_margin = 0.0; // Initial margin requirement
    double maintenance_margin = 0.0; // Maintenance margin requirement
    double price_limit_up = 0.0; // Daily price limit up (0 = no limit)
    double price_limit_down = 0.0; // Daily price limit down (0 = no limit)
    uint32_t position_limit = 0; // Position limit (0 = no limit)
    double max_order_value = 0.0; // Maximum order value (0 = no limit)
};

// Complete instrument definition
struct InstrumentDefinition {
    // Basic identification
    std::string symbol; // ES, NQ, etc.
    uint32_t security_id; // Unique security ID
    std::string name; // Full instrument name
    std::string description; // Detailed description

    // Classification
    InstrumentType instrument_type = InstrumentType::Future;
    ProductGroup product_group = ProductGroup::Equity;
    std::string exchange = "CME"; // Exchange code
    std::string currency = "USD"; // Settlement currency

    // Contract specifications
    std::string underlying_symbol; // Underlying asset symbol
    double contract_size = 1.0; // Contract multiplier
    std::string maturity_date; // Contract expiry (YYYY-MM-DD)
    std::string first_notice_date; // First notice date for delivery
    std::string last_trading_date; // Last trading date

    // Price formatting
    PriceFormat price_format;

    // Trading parameters
    std::vector<TradingSession> sessions;
    RiskParameters risk_params;
    TradingStatus trading_status = TradingStatus::Trading;

    // Market making parameters
    double min_quote_size = 1.0; // Minimum quote size
    double round_lot_size = 1.0; // Standard trading unit
    uint32_t max_book_depth = 10; // Maximum book depth

    // Reference prices for initialization
    double reference_price = 0.0; // Current reference price
    double opening_price = 0.0; // Today's opening price
    double high_price = 0.0; // Today's high price
    double low_price = 0.0; // Today's low price
    double previous_settle = 0.0; // Previous day's settlement

    // Volume and open interest
    uint64_t volume = 0; // Today's volume
    uint64_t open_interest = 0; // Open interest

    // Timestamps
    std::chrono::system_clock::time_point last_update_time;
    std::chrono::system_clock::time_point creation_time;
};

// Market session manager
class SessionManager {
public:
    SessionManager();

    // Session management
    void add_session(const std::string& instrument_symbol, const TradingSession& session);
    SessionState get_current_session_state(const std::string& instrument_symbol) const;
    TradingSession get_current_session(const std::string& instrument_symbol) const;
    std::vector<TradingSession> get_sessions(const std::string& instrument_symbol) const;

    // Time utilities
    bool is_trading_hours(const std::string& instrument_symbol) const;
    std::chrono::system_clock::time_point get_next_session_start(const std::string& instrument_symbol) const;
    std::chrono::system_clock::time_point get_session_end(const std::string& instrument_symbol) const;

private:
    std::map<std::string, std::vector<TradingSession>> instrument_sessions_;
};

// Main reference data manager
class ReferenceDataManager {
public:
    static ReferenceDataManager& instance();

    // Initialization
    bool initialize();
    bool load_from_config(const std::string& config_file);
    bool load_instruments(const std::vector<struct InstrumentConfig>& configs);

    // Instrument lookup
    const InstrumentDefinition* get_instrument(uint32_t security_id) const;
    const InstrumentDefinition* get_instrument(const std::string& symbol) const;
    std::vector<const InstrumentDefinition*> get_all_instruments() const;
    std::vector<const InstrumentDefinition*> get_instruments_by_group(ProductGroup group) const;

    // Instrument management
    bool add_instrument(const InstrumentDefinition& instrument);
    bool update_instrument(uint32_t security_id, const InstrumentDefinition& instrument);
    bool remove_instrument(uint32_t security_id);

    // Price utilities
    double round_price(uint32_t security_id, double price) const;
    double get_tick_size(uint32_t security_id) const;
    double get_tick_value(uint32_t security_id) const;
    double calculate_tick_value(uint32_t security_id, double price_diff) const;

    // Trading status
    void set_trading_status(uint32_t security_id, TradingStatus status);
    TradingStatus get_trading_status(uint32_t security_id) const;
    bool is_trading_allowed(uint32_t security_id) const;

    // Session management
    SessionManager& session_manager() { return session_manager_; }
    const SessionManager& session_manager() const { return session_manager_; }

    // Statistics
    size_t get_instrument_count() const { return instruments_.size(); }
    std::vector<std::string> get_supported_symbols() const;

    // Reference price management
    void update_reference_price(uint32_t security_id, double price);
    void update_daily_stats(uint32_t security_id, double high, double low, uint64_t volume);

private:
    ReferenceDataManager() = default;

    // Create default CME instruments (Channel 310 equity products)
    void create_default_instruments();
    InstrumentDefinition create_es_future() const; // E-mini S&P 500
    InstrumentDefinition create_nq_future() const; // E-mini NASDAQ-100
    InstrumentDefinition create_ym_future() const; // E-mini Dow Jones
    InstrumentDefinition create_rty_future() const; // E-mini Russell 2000
    InstrumentDefinition create_emd_future() const; // E-mini S&P MidCap 400
    InstrumentDefinition create_mes_future() const; // Micro E-mini S&P 500
    InstrumentDefinition create_mnq_future() const; // Micro E-mini NASDAQ-100
    InstrumentDefinition create_mym_future() const; // Micro E-mini Dow Jones
    InstrumentDefinition create_m2k_future() const; // Micro E-mini Russell 2000
    InstrumentDefinition create_vx_future() const; // VIX Futures

    // Setup trading sessions
    void setup_default_sessions();
    TradingSession create_rth_session() const; // Regular Trading Hours
    TradingSession create_eth_session() const; // Electronic Trading Hours

    std::map<uint32_t, InstrumentDefinition> instruments_; // By security_id
    std::map<std::string, uint32_t> symbol_to_id_; // Symbol lookup
    SessionManager session_manager_;
};

} // namespace cme_mock