#include "core/reference_data.h"
#include "utils/logger.h"
#include <iomanip>
#include <iostream>

using namespace cme_mock;

int main()
{
    std::cout << "CME Channels 310 & 330 - All Instruments" << std::endl;
    std::cout << "=========================================" << std::endl;

    // Initialize logger
    Logger::instance().set_level(LogLevel::WARNING); // Minimize log output

    // Initialize reference data manager
    auto& ref_data = ReferenceDataManager::instance();
    if (!ref_data.initialize()) {
        std::cerr << "Failed to initialize Reference Data Manager" << std::endl;
        return 1;
    }

    std::cout << "Total Instruments: " << ref_data.get_instrument_count() << std::endl;
    std::cout << std::endl;

    // Print header
    std::cout << std::left
              << std::setw(6) << "ID"
              << std::setw(8) << "Symbol"
              << std::setw(35) << "Name"
              << std::setw(10) << "Price"
              << std::setw(10) << "Tick Size"
              << std::setw(12) << "Multiplier"
              << std::setw(10) << "Exchange"
              << std::endl;

    std::cout << std::string(90, '-') << std::endl;

    // Get all instruments and display them
    auto instruments = ref_data.get_all_instruments();
    for (const auto* instrument : instruments) {
        std::cout << std::left
                  << std::setw(6) << instrument->security_id
                  << std::setw(8) << instrument->symbol
                  << std::setw(35) << instrument->name
                  << std::setw(10) << std::fixed << std::setprecision(2) << instrument->reference_price
                  << std::setw(10) << std::fixed << std::setprecision(2) << instrument->price_format.tick_size
                  << std::setw(12) << std::fixed << std::setprecision(1) << instrument->contract_size
                  << std::setw(10) << instrument->exchange
                  << std::endl;
    }

    std::cout << std::endl;

    // Show contract specifications for major products
    std::cout << "Major Contract Specifications:" << std::endl;
    std::cout << "=============================" << std::endl;

    std::vector<std::string> major_symbols = { "ESZ4", "NQZ4", "YMZ4", "RTYZ4", "VXZ4" };

    for (const auto& symbol : major_symbols) {
        const auto* instrument = ref_data.get_instrument(symbol);
        if (instrument) {
            std::cout << "\n"
                      << instrument->symbol << " - " << instrument->name << std::endl;
            std::cout << "  Contract Size: $" << instrument->contract_size << " per index point" << std::endl;
            std::cout << "  Tick Size: " << instrument->price_format.tick_size << " points" << std::endl;
            std::cout << "  Tick Value: $" << instrument->price_format.tick_value << std::endl;
            std::cout << "  Initial Margin: $" << instrument->risk_params.initial_margin << std::endl;
            std::cout << "  Underlying: " << instrument->underlying_symbol << std::endl;
        }
    }

    std::cout << "\nMicro E-mini Contracts (1/10th size):" << std::endl;
    std::cout << "====================================" << std::endl;

    std::vector<std::string> micro_symbols = { "MESZ4", "MNQZ4", "MYMZ4", "M2KZ4" };

    for (const auto& symbol : micro_symbols) {
        const auto* instrument = ref_data.get_instrument(symbol);
        if (instrument) {
            std::cout << "\n"
                      << instrument->symbol << " - " << instrument->name << std::endl;
            std::cout << "  Contract Size: $" << instrument->contract_size << " per index point" << std::endl;
            std::cout << "  Tick Value: $" << instrument->price_format.tick_value << std::endl;
            std::cout << "  Initial Margin: $" << instrument->risk_params.initial_margin << std::endl;
        }
    }

    std::cout << "\nFX Spot Plus Contracts (Channel 330):" << std::endl;
    std::cout << "=====================================" << std::endl;

    std::vector<std::string> fx_symbols = { "EURUSD", "GBPUSD", "USDJPY", "USDCAD" };

    for (const auto& symbol : fx_symbols) {
        const auto* instrument = ref_data.get_instrument(symbol);
        if (instrument) {
            std::cout << "\n"
                      << instrument->symbol << " - " << instrument->name << std::endl;
            std::cout << "  Tick Size: " << std::fixed << std::setprecision(5) << instrument->price_format.tick_size << std::endl;
            std::cout << "  Initial Price: " << std::fixed << std::setprecision(5) << instrument->reference_price << std::endl;
            std::cout << "  Initial Margin: $" << instrument->risk_params.initial_margin << std::endl;
            std::cout << "  Underlying: " << instrument->underlying_symbol << std::endl;
        }
    }

    std::cout << std::endl;

    return 0;
}