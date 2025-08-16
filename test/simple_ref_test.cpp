#include "core/reference_data.h"
#include "utils/logger.h"
#include <iostream>

using namespace cme_mock;

int main()
{
    std::cout << "Simple Reference Data Test" << std::endl;

    // Initialize logger
    Logger::instance().set_level(LogLevel::INFO);

    // Initialize reference data manager
    auto& ref_data = ReferenceDataManager::instance();
    std::cout << "Calling initialize..." << std::endl;

    if (!ref_data.initialize()) {
        std::cerr << "Failed to initialize Reference Data Manager" << std::endl;
        return 1;
    }

    std::cout << "Initialized successfully" << std::endl;
    std::cout << "Total instruments: " << ref_data.get_instrument_count() << std::endl;

    // Test simple lookup
    std::cout << "Testing ES lookup..." << std::endl;
    const auto* es = ref_data.get_instrument("ES");
    if (es) {
        std::cout << "Found ES: " << es->name << std::endl;
    } else {
        std::cout << "ES not found" << std::endl;
    }

    std::cout << "Test completed!" << std::endl;
    return 0;
}