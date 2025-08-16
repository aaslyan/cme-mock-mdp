#include "config/configuration.h"
#include "core/reference_data.h"
#include "utils/logger.h"
#include <iomanip>
#include <iostream>

using namespace cme_mock;

void print_instrument_details(const InstrumentDefinition* instrument)
{
    if (!instrument) {
        std::cout << "  Instrument not found" << std::endl;
        return;
    }

    std::cout << "  Symbol: " << instrument->symbol << std::endl;
    std::cout << "  Security ID: " << instrument->security_id << std::endl;
    std::cout << "  Name: " << instrument->name << std::endl;
    std::cout << "  Description: " << instrument->description << std::endl;
    std::cout << "  Exchange: " << instrument->exchange << std::endl;
    std::cout << "  Currency: " << instrument->currency << std::endl;
    std::cout << "  Contract Size: " << instrument->contract_size << std::endl;
    std::cout << "  Tick Size: " << instrument->price_format.tick_size << std::endl;
    std::cout << "  Tick Value: $" << std::fixed << std::setprecision(2)
              << instrument->price_format.tick_value << std::endl;
    std::cout << "  Point Value: $" << std::fixed << std::setprecision(2)
              << instrument->price_format.point_value << std::endl;
    std::cout << "  Reference Price: $" << std::fixed << std::setprecision(2)
              << instrument->reference_price << std::endl;
    std::cout << "  Initial Margin: $" << std::fixed << std::setprecision(2)
              << instrument->risk_params.initial_margin << std::endl;

    // Product group
    std::string group_name;
    switch (instrument->product_group) {
    case ProductGroup::Equity:
        group_name = "Equity";
        break;
    case ProductGroup::Energy:
        group_name = "Energy";
        break;
    case ProductGroup::Metals:
        group_name = "Metals";
        break;
    case ProductGroup::Interest:
        group_name = "Interest Rate";
        break;
    case ProductGroup::FX:
        group_name = "Foreign Exchange";
        break;
    case ProductGroup::Agricultural:
        group_name = "Agricultural";
        break;
    }
    std::cout << "  Product Group: " << group_name << std::endl;

    // Trading status
    std::string status_name;
    switch (instrument->trading_status) {
    case TradingStatus::Trading:
        status_name = "Trading";
        break;
    case TradingStatus::Halted:
        status_name = "Halted";
        break;
    case TradingStatus::PreOpen:
        status_name = "Pre-Open";
        break;
    case TradingStatus::NoCancel:
        status_name = "No Cancel";
        break;
    default:
        status_name = "Unknown";
        break;
    }
    std::cout << "  Trading Status: " << status_name << std::endl;
}

void test_price_utilities()
{
    std::cout << "\n=== Testing Price Utilities ===" << std::endl;

    auto& ref_data = ReferenceDataManager::instance();

    // Test price rounding for ES
    uint32_t es_id = 1;
    std::vector<double> test_prices = { 4500.123, 4500.24, 4500.26, 4500.37, 4500.49 };

    std::cout << "ES Price Rounding (tick size: " << ref_data.get_tick_size(es_id) << "):" << std::endl;
    for (double price : test_prices) {
        double rounded = ref_data.round_price(es_id, price);
        std::cout << "  " << std::fixed << std::setprecision(3) << price
                  << " -> " << std::setprecision(2) << rounded << std::endl;
    }

    // Test tick value calculations
    std::cout << "\nTick Value Calculations:" << std::endl;
    double price_diff = 1.25; // 5 ticks for ES
    double tick_value = ref_data.calculate_tick_value(es_id, price_diff);
    std::cout << "  ES: $" << std::fixed << std::setprecision(2) << price_diff
              << " price diff = $" << tick_value << " value" << std::endl;

    // Test for other instruments
    uint32_t cl_id = 3;
    price_diff = 0.05; // 5 ticks for CL
    tick_value = ref_data.calculate_tick_value(cl_id, price_diff);
    std::cout << "  CL: $" << std::fixed << std::setprecision(2) << price_diff
              << " price diff = $" << tick_value << " value" << std::endl;
}

void test_trading_status()
{
    std::cout << "\n=== Testing Trading Status ===" << std::endl;

    auto& ref_data = ReferenceDataManager::instance();

    uint32_t es_id = 1;

    std::cout << "Initial ES trading status: "
              << (ref_data.is_trading_allowed(es_id) ? "Trading Allowed" : "Trading Halted") << std::endl;

    // Set to halted
    ref_data.set_trading_status(es_id, TradingStatus::Halted);
    std::cout << "After halt: "
              << (ref_data.is_trading_allowed(es_id) ? "Trading Allowed" : "Trading Halted") << std::endl;

    // Set back to trading
    ref_data.set_trading_status(es_id, TradingStatus::Trading);
    std::cout << "After resume: "
              << (ref_data.is_trading_allowed(es_id) ? "Trading Allowed" : "Trading Halted") << std::endl;
}

void test_instrument_lookup()
{
    std::cout << "\n=== Testing Instrument Lookup ===" << std::endl;

    auto& ref_data = ReferenceDataManager::instance();

    // Test lookup by symbol
    std::cout << "Looking up ES by symbol:" << std::endl;
    const auto* es = ref_data.get_instrument("ES");
    print_instrument_details(es);

    // Test lookup by security ID
    std::cout << "\nLooking up security ID 3 (CL):" << std::endl;
    const auto* cl = ref_data.get_instrument(3);
    print_instrument_details(cl);

    // Test invalid lookup
    std::cout << "\nLooking up invalid symbol 'INVALID':" << std::endl;
    const auto* invalid = ref_data.get_instrument("INVALID");
    print_instrument_details(invalid);
}

void test_product_groups()
{
    std::cout << "\n=== Testing Product Groups ===" << std::endl;

    auto& ref_data = ReferenceDataManager::instance();

    // Get all equity instruments
    auto equity_instruments = ref_data.get_instruments_by_group(ProductGroup::Equity);
    std::cout << "Equity instruments (" << equity_instruments.size() << "):" << std::endl;
    for (const auto* instrument : equity_instruments) {
        std::cout << "  " << instrument->symbol << " - " << instrument->name << std::endl;
    }

    // Get all energy instruments
    auto energy_instruments = ref_data.get_instruments_by_group(ProductGroup::Energy);
    std::cout << "Energy instruments (" << energy_instruments.size() << "):" << std::endl;
    for (const auto* instrument : energy_instruments) {
        std::cout << "  " << instrument->symbol << " - " << instrument->name << std::endl;
    }

    // Get all metals instruments
    auto metals_instruments = ref_data.get_instruments_by_group(ProductGroup::Metals);
    std::cout << "Metals instruments (" << metals_instruments.size() << "):" << std::endl;
    for (const auto* instrument : metals_instruments) {
        std::cout << "  " << instrument->symbol << " - " << instrument->name << std::endl;
    }
}

void test_configuration_integration()
{
    std::cout << "\n=== Testing Configuration Integration ===" << std::endl;

    auto& config = Configuration::instance();
    auto& ref_data = ReferenceDataManager::instance();

    // Load instruments from configuration
    ref_data.load_instruments(config.instruments());

    std::cout << "Loaded " << ref_data.get_instrument_count() << " instruments from configuration" << std::endl;

    // Verify symbols match
    auto supported_symbols = ref_data.get_supported_symbols();
    std::cout << "Supported symbols: ";
    for (size_t i = 0; i < supported_symbols.size(); ++i) {
        std::cout << supported_symbols[i];
        if (i < supported_symbols.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
}

int main()
{
    std::cout << "Reference Data Manager Test" << std::endl;
    std::cout << "===========================" << std::endl;

    // Initialize logger
    Logger::instance().set_level(LogLevel::INFO);

    // Load configuration
    Configuration::instance().load_from_file("config/server_config.json");

    // Initialize reference data manager
    auto& ref_data = ReferenceDataManager::instance();
    if (!ref_data.initialize()) {
        std::cerr << "Failed to initialize Reference Data Manager" << std::endl;
        return 1;
    }

    std::cout << "Reference Data Manager initialized successfully" << std::endl;
    std::cout << "Total instruments: " << ref_data.get_instrument_count() << std::endl;

    // Run tests
    test_instrument_lookup();
    test_price_utilities();
    test_trading_status();
    test_product_groups();
    test_configuration_integration();

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "All reference data tests completed successfully!" << std::endl;

    return 0;
}