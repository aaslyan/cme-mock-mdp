#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "scenarios/market_scenario.h"
#include "utils/logger.h"

using namespace cme_mock;

void test_scenario(ScenarioType type, int duration_seconds = 5) {
    auto& scenario_mgr = ScenarioManager::instance();
    
    std::cout << "\n=== Testing " << scenario_mgr.get_scenario_name(type) 
              << " Scenario ===" << std::endl;
    std::cout << scenario_mgr.get_scenario_description(type) << std::endl;
    
    // Create scenario
    auto config = scenario_mgr.get_default_config(type);
    config.duration = std::chrono::seconds(duration_seconds);
    auto scenario = scenario_mgr.create_scenario(config);
    
    // Initialize and start
    scenario->initialize();
    scenario->start();
    
    std::cout << "Running for " << duration_seconds << " seconds..." << std::endl;
    
    // Run for specified duration
    auto start = std::chrono::steady_clock::now();
    while (scenario->is_active()) {
        scenario->update();
        
        // Check progress
        double progress = scenario->get_progress();
        if (progress > 0) {
            std::cout << "\rProgress: " << int(progress * 100) << "%  " << std::flush;
        }
        
        // Check if duration elapsed
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed >= std::chrono::seconds(duration_seconds)) {
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    scenario->stop();
    std::cout << "\nScenario completed.\n" << std::endl;
}

int main() {
    Logger::instance().set_level(LogLevel::INFO);
    
    std::cout << "CME Mock Server - Scenario Testing" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // List available scenarios
    auto& scenario_mgr = ScenarioManager::instance();
    std::cout << "\nAvailable scenarios:" << std::endl;
    for (auto type : scenario_mgr.get_available_scenarios()) {
        std::cout << "  - " << scenario_mgr.get_scenario_name(type) << std::endl;
    }
    
    // Test each scenario
    test_scenario(ScenarioType::Normal, 3);
    test_scenario(ScenarioType::FastMarket, 3);
    test_scenario(ScenarioType::ThinMarket, 3);
    test_scenario(ScenarioType::Recovery, 10);  // Longer to see gap simulation
    
    std::cout << "\nAll scenario tests completed!" << std::endl;
    
    return 0;
}