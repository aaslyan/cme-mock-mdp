#include "../../core/include/market_data_generator.h"
#include "../../core/include/order_book_manager.h"
#include "../../protocols/reuters/include/reuters_protocol_adapter.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <signal.h>
#include <thread>

std::atomic<bool> running(true);

void signal_handler(int signal)
{
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

int main(int argc, char* argv[])
{
    // Install signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::cout << "Reuters FX Market Data Server" << std::endl;
    std::cout << "=============================" << std::endl;

    try {
        // Initialize core market data system (same as CME server)
        auto book_manager = std::make_shared<market_core::OrderBookManager>();
        auto data_generator = std::make_shared<market_core::MarketDataGenerator>(book_manager);

        // Create sample FX instruments using our core system
        std::cout << "Creating FX instruments..." << std::endl;

        // EUR/USD
        auto eurusd = std::make_shared<market_core::Instrument>(1001, "EURUSD", market_core::InstrumentType::FX_SPOT);
        eurusd->tick_size = 0.00001;
        eurusd->set_property("initial_price", 1.0850);
        eurusd->set_property("initial_spread", 0.00002);
        book_manager->add_instrument(eurusd);

        // GBP/USD
        auto gbpusd = std::make_shared<market_core::Instrument>(1002, "GBPUSD", market_core::InstrumentType::FX_SPOT);
        gbpusd->tick_size = 0.00001;
        gbpusd->set_property("initial_price", 1.2650);
        gbpusd->set_property("initial_spread", 0.00003);
        book_manager->add_instrument(gbpusd);

        // USD/JPY
        auto usdjpy = std::make_shared<market_core::Instrument>(1003, "USDJPY", market_core::InstrumentType::FX_SPOT);
        usdjpy->tick_size = 0.001;
        usdjpy->set_property("initial_price", 149.50);
        usdjpy->set_property("initial_spread", 0.002);
        book_manager->add_instrument(usdjpy);

        std::cout << "Created " << book_manager->get_all_instrument_ids().size() << " FX instruments" << std::endl;

        // Initialize market data with realistic FX volatility
        data_generator->set_market_mode(market_core::MarketMode::NORMAL);

        // Generate initial market state
        std::cout << "Generating initial market state..." << std::endl;
        data_generator->generate_all_instruments();

        // Initialize Reuters protocol adapter
        uint16_t port = 11501; // Same as sample client
        if (argc > 1 && std::string(argv[1]) != "--help") {
            try {
                port = static_cast<uint16_t>(std::stoi(argv[1]));
            } catch (const std::exception&) {
                std::cerr << "Invalid port number: " << argv[1] << std::endl;
                std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
                std::cout << "Default port: 11501" << std::endl;
                return 1;
            }
        } else if (argc > 1 && std::string(argv[1]) == "--help") {
            std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
            std::cout << "  port: TCP port to listen on (default: 11501)" << std::endl;
            std::cout << "  Implements LSEG FX Market Data API over TCP with SBE encoding" << std::endl;
            return 0;
        }

        auto reuters_adapter = std::make_unique<reuters_protocol::ReutersProtocolAdapter>(port);

        // Connect market data generator to Reuters adapter
        // Convert raw pointer to shared_ptr for listener management
        std::shared_ptr<reuters_protocol::ReutersProtocolAdapter> reuters_shared(reuters_adapter.release());
        data_generator->add_listener(reuters_shared);

        // Initialize Reuters server
        if (!reuters_shared->initialize()) {
            std::cerr << "Failed to initialize Reuters server on port " << port << std::endl;
            return 1;
        }

        std::cout << "Reuters server listening on port " << port << std::endl;
        std::cout << "Protocol: TCP with LSEG FX Market Data API (SBE)" << std::endl;
        std::cout << "Market data: Real-time FX quotes and trades" << std::endl;
        std::cout << "Press Ctrl+C to shutdown" << std::endl;
        std::cout << std::endl;

        // Main server loop (single-threaded as requested)
        auto last_market_update = std::chrono::steady_clock::now();
        auto last_stats_print = std::chrono::steady_clock::now();

        while (running) {
            auto now = std::chrono::steady_clock::now();

            // Process Reuters protocol (TCP connections, sessions, messages)
            reuters_shared->run_once();

            // Generate market data updates every 100ms
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_market_update).count() >= 100) {
                data_generator->generate_all_instruments();
                last_market_update = now;
            }

            // Print statistics every 10 seconds
            if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_print).count() >= 10) {
                const auto& stats = reuters_shared->get_statistics();
                auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - stats.start_time).count();

                std::cout << "Stats [" << uptime << "s]: "
                          << "Sessions=" << stats.sessions_created
                          << ", Sent=" << stats.messages_sent
                          << ", Recv=" << stats.messages_received
                          << ", Events=" << stats.market_events_processed
                          << std::endl;

                last_stats_print = now;
            }

            // Small sleep to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::cout << "\nShutting down Reuters server..." << std::endl;
        reuters_shared->shutdown();

        // Print final statistics
        const auto& final_stats = reuters_shared->get_statistics();
        auto total_uptime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - final_stats.start_time)
                                .count();

        std::cout << "Final Statistics:" << std::endl;
        std::cout << "  Uptime: " << total_uptime << " seconds" << std::endl;
        std::cout << "  Sessions created: " << final_stats.sessions_created << std::endl;
        std::cout << "  Messages sent: " << final_stats.messages_sent << std::endl;
        std::cout << "  Messages received: " << final_stats.messages_received << std::endl;
        std::cout << "  Market events processed: " << final_stats.market_events_processed << std::endl;

        std::cout << "Reuters server shutdown complete." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}