#include "../../core/include/instrument.h"
#include "../../core/include/market_data_generator.h"
#include "../../core/include/order_book_manager.h"
#include "../../protocols/cme/include/cme_event_listener.h"
#include "../../protocols/cme/include/cme_protocol_adapter.h"
#include "../../protocols/common/include/udp_transport.h"

#include <atomic>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <thread>
#include <vector>

// Global shutdown flag
std::atomic<bool> g_running { true };

void signal_handler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutdown signal received..." << std::endl;
        g_running = false;
    }
}

void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -i, --incremental-ip IP    Incremental feed IP (default: 224.0.28.64)\n"
              << "  -p, --incremental-port P   Incremental feed port (default: 14310)\n"
              << "  -s, --snapshot-ip IP       Snapshot feed IP (default: 224.0.28.69)\n"
              << "  -q, --snapshot-port P      Snapshot feed port (default: 14320)\n"
              << "  -m, --mode MODE           Market mode: normal, fast, volatile, thin (default: normal)\n"
              << "  -r, --rate N              Updates per second (default: 10)\n"
              << "  -v, --verbose             Enable verbose logging\n"
              << "  -h, --help                Show this help message\n\n"
              << "Examples:\n"
              << "  " << program_name << " --mode fast --rate 50\n"
              << "  " << program_name << " --incremental-ip 127.0.0.1 --incremental-port 20001\n";
}

// Create sample futures instruments
std::vector<std::shared_ptr<market_core::Instrument>> create_sample_instruments()
{
    std::vector<std::shared_ptr<market_core::Instrument>> instruments;

    // E-mini S&P 500 Futures
    {
        auto es = std::make_shared<market_core::FuturesInstrument>(1, "ESZ4");
        es->description = "E-mini S&P 500 Dec 2024";
        es->tick_size = 0.25;
        es->multiplier = 50.0;
        es->underlying = "SPX";
        es->maturity_date = "2024-12-20";
        es->contract_size = 50.0;
        es->initial_margin = 13200.0;
        es->maintenance_margin = 12000.0;

        // Set CME-specific properties
        es->set_property("initial_price", 4500.0);
        es->set_property("price_decimals", int64_t(2));
        es->set_property("currency", std::string("USD"));
        es->external_ids["CME_SECURITY_ID"] = "1";

        instruments.push_back(es);
    }

    // Micro E-mini S&P 500
    {
        auto mes = std::make_shared<market_core::FuturesInstrument>(2, "MESZ4");
        mes->description = "Micro E-mini S&P 500 Dec 2024";
        mes->tick_size = 0.25;
        mes->multiplier = 5.0;
        mes->underlying = "SPX";
        mes->maturity_date = "2024-12-20";
        mes->contract_size = 5.0;
        mes->initial_margin = 1320.0;
        mes->maintenance_margin = 1200.0;

        mes->set_property("initial_price", 4500.0);
        mes->set_property("price_decimals", int64_t(2));
        mes->set_property("currency", std::string("USD"));
        mes->external_ids["CME_SECURITY_ID"] = "2";

        instruments.push_back(mes);
    }

    // E-mini NASDAQ 100
    {
        auto nq = std::make_shared<market_core::FuturesInstrument>(3, "NQZ4");
        nq->description = "E-mini NASDAQ 100 Dec 2024";
        nq->tick_size = 0.25;
        nq->multiplier = 20.0;
        nq->underlying = "NDX";
        nq->maturity_date = "2024-12-20";
        nq->contract_size = 20.0;
        nq->initial_margin = 19800.0;
        nq->maintenance_margin = 18000.0;

        nq->set_property("initial_price", 15000.0);
        nq->set_property("price_decimals", int64_t(2));
        nq->set_property("currency", std::string("USD"));
        nq->external_ids["CME_SECURITY_ID"] = "3";

        instruments.push_back(nq);
    }

    return instruments;
}

int main(int argc, char* argv[])
{
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Default configuration
    std::string incremental_ip = "224.0.28.64";
    uint16_t incremental_port = 14310;
    std::string snapshot_ip = "224.0.28.69";
    uint16_t snapshot_port = 14320;
    market_core::MarketMode market_mode = market_core::MarketMode::NORMAL;
    int updates_per_second = 10;
    bool verbose = false;

    // Parse command line arguments
    static struct option long_options[] = {
        { "incremental-ip", required_argument, 0, 'i' },
        { "incremental-port", required_argument, 0, 'p' },
        { "snapshot-ip", required_argument, 0, 's' },
        { "snapshot-port", required_argument, 0, 'q' },
        { "mode", required_argument, 0, 'm' },
        { "rate", required_argument, 0, 'r' },
        { "verbose", no_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "i:p:s:q:m:r:vh", long_options, nullptr)) != -1) {
        switch (opt) {
        case 'i':
            incremental_ip = optarg;
            break;
        case 'p':
            incremental_port = static_cast<uint16_t>(std::stoi(optarg));
            break;
        case 's':
            snapshot_ip = optarg;
            break;
        case 'q':
            snapshot_port = static_cast<uint16_t>(std::stoi(optarg));
            break;
        case 'm': {
            std::string mode_str = optarg;
            if (mode_str == "fast")
                market_mode = market_core::MarketMode::FAST;
            else if (mode_str == "volatile")
                market_mode = market_core::MarketMode::VOLATILE;
            else if (mode_str == "thin")
                market_mode = market_core::MarketMode::THIN;
            else if (mode_str == "trending")
                market_mode = market_core::MarketMode::TRENDING;
            else if (mode_str == "stressed")
                market_mode = market_core::MarketMode::STRESSED;
            break;
        }
        case 'r':
            updates_per_second = std::stoi(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    std::cout << "CME Mock MDP Server (New Architecture)\n";
    std::cout << "=====================================\n";
    std::cout << "Incremental Feed: " << incremental_ip << ":" << incremental_port << "\n";
    std::cout << "Snapshot Feed:    " << snapshot_ip << ":" << snapshot_port << "\n";
    std::cout << "Market Mode:      ";
    switch (market_mode) {
    case market_core::MarketMode::NORMAL:
        std::cout << "Normal";
        break;
    case market_core::MarketMode::FAST:
        std::cout << "Fast";
        break;
    case market_core::MarketMode::VOLATILE:
        std::cout << "Volatile";
        break;
    case market_core::MarketMode::THIN:
        std::cout << "Thin";
        break;
    case market_core::MarketMode::TRENDING:
        std::cout << "Trending";
        break;
    case market_core::MarketMode::STRESSED:
        std::cout << "Stressed";
        break;
    }
    std::cout << "\n";
    std::cout << "Update Rate:      " << updates_per_second << " Hz\n\n";

    try {
        // 1. Create core components
        auto book_manager = std::make_shared<market_core::OrderBookManager>();
        auto market_generator = std::make_shared<market_core::MarketDataGenerator>(book_manager);

        // 2. Create and configure instruments
        auto instruments = create_sample_instruments();
        for (auto& instrument : instruments) {
            book_manager->add_instrument(instrument);
            book_manager->create_order_book(instrument->instrument_id);
            std::cout << "Added instrument: " << instrument->primary_symbol
                      << " (ID: " << instrument->instrument_id << ")\n";
        }

        // 3. Set up market generator
        market_generator->set_market_mode(market_mode);
        auto config = market_generator->get_config();
        config.updates_per_second = updates_per_second;
        market_generator->set_config(config);

        // 4. Create CME protocol adapters and transports
        auto incremental_adapter = std::make_shared<cme_protocol::CMEProtocolAdapter>();
        auto snapshot_adapter = std::make_shared<cme_protocol::CMEProtocolAdapter>();

        auto inc_transport = std::make_shared<market_protocols::UDPTransport>(
            incremental_ip, incremental_port);
        auto snap_transport = std::make_shared<market_protocols::UDPTransport>(
            snapshot_ip, snapshot_port);

        if (!inc_transport->initialize() || !snap_transport->initialize()) {
            std::cerr << "Failed to initialize UDP transports\n";
            return 1;
        }

        incremental_adapter->set_transport(inc_transport);
        incremental_adapter->set_channel_id(310); // CME Equity Futures
        incremental_adapter->set_batch_size(5);

        snapshot_adapter->set_transport(snap_transport);
        snapshot_adapter->set_channel_id(310);
        snapshot_adapter->set_batch_size(1);

        // 5. Create event listeners to connect core to protocols
        auto inc_listener = std::make_shared<cme_protocol::CMEEventListener>(
            book_manager, incremental_adapter);
        auto snap_listener = std::make_shared<cme_protocol::CMEEventListener>(
            book_manager, snapshot_adapter);

        // 6. Register listeners with market generator
        market_generator->add_listener(inc_listener);
        market_generator->add_listener(snap_listener);

        // 7. Send instrument definitions
        std::cout << "\nSending instrument definitions...\n";
        for (auto& instrument : instruments) {
            incremental_adapter->send_instrument_definition(*instrument);
            if (verbose) {
                std::cout << "Sent definition for " << instrument->primary_symbol << "\n";
            }
        }

        // 8. Generate initial snapshots
        std::cout << "Generating initial snapshots...\n";
        for (auto& instrument : instruments) {
            auto snapshot = market_generator->generate_snapshot(instrument->instrument_id);
            if (snapshot && verbose) {
                std::cout << "Generated snapshot for " << instrument->primary_symbol << "\n";
            }
        }

        std::cout << "\nStarting market data generation...\n";
        std::cout << "Press Ctrl+C to stop\n\n";

        // 9. Main market data generation loop
        auto last_snapshot = std::chrono::steady_clock::now();
        const auto snapshot_interval = std::chrono::seconds(30);
        const auto update_interval = std::chrono::milliseconds(1000 / updates_per_second);

        auto stats_timer = std::chrono::steady_clock::now();
        const auto stats_interval = std::chrono::seconds(10);

        while (g_running) {
            auto loop_start = std::chrono::steady_clock::now();

            // Generate market updates
            market_generator->generate_all_instruments();

            // Periodic snapshots
            if (loop_start - last_snapshot >= snapshot_interval) {
                for (auto& instrument : instruments) {
                    market_generator->generate_snapshot(instrument->instrument_id);
                }
                last_snapshot = loop_start;
                if (verbose) {
                    std::cout << "Generated periodic snapshots\n";
                }
            }

            // Print statistics
            if (loop_start - stats_timer >= stats_interval) {
                const auto& stats = market_generator->get_statistics();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    loop_start - stats.start_time)
                                   .count();

                std::cout << "Stats: " << stats.updates_generated << " updates, "
                          << stats.trades_generated << " trades, "
                          << stats.quotes_generated << " quotes, "
                          << stats.snapshots_generated << " snapshots "
                          << "(" << (elapsed > 0 ? stats.updates_generated / elapsed : 0)
                          << " updates/sec)\n";

                stats_timer = loop_start;
            }

            // Sleep to maintain update rate
            auto loop_end = std::chrono::steady_clock::now();
            auto elapsed = loop_end - loop_start;
            if (elapsed < update_interval) {
                std::this_thread::sleep_for(update_interval - elapsed);
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nShutdown complete.\n";
    return 0;
}