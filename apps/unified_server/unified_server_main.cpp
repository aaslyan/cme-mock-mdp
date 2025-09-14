#include "../../core/include/instrument.h"
#include "../../core/include/market_data_generator.h"
#include "../../core/include/order_book_manager.h"
#include "../../protocols/cme/include/cme_event_listener.h"
#include "../../protocols/cme/include/cme_protocol_adapter.h"
#include "../../protocols/reuters/include/reuters_protocol_adapter.h"
#include "../../protocols/utp/include/utp_protocol_adapter.h"
#include "../../protocols/common/include/udp_transport.h"
#include "../../protocols/common/include/udp_multicast_transport.h"
#include "../../protocols/common/include/tcp_transport.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <signal.h>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
              << "  -c, --config FILE         Configuration file (default: config/unified_server_config.json)\n"
              << "  -p, --protocol PROTOCOL   Override protocol: cme, reuters, utp (default: from config)\n"
              << "  -m, --mode MODE           Market mode: normal, fast, volatile, trending (default: from config)\n"
              << "  -r, --rate N              Updates per second (default: from config)\n"
              << "  -v, --verbose             Enable verbose logging\n"
              << "  -h, --help                Show this help message\n\n"
              << "Examples:\n"
              << "  " << program_name << " --protocol cme --mode volatile --rate 50\n"
              << "  " << program_name << " --config my_config.json --protocol utp\n"
              << "  " << program_name << " --protocol reuters --mode normal\n";
}

struct ServerConfig {
    std::string protocol = "cme";
    std::string market_mode = "normal";
    int updates_per_second = 10;
    int snapshot_interval_seconds = 30;
    int heartbeat_interval_seconds = 10;
    int max_book_depth = 10;
    
    struct NetworkConfig {
        std::string incremental_ip = "224.0.28.64";
        uint16_t incremental_port = 14310;
        std::string snapshot_ip = "224.0.28.69";
        uint16_t snapshot_port = 14320;
        std::string definition_ip = "224.0.28.70";
        uint16_t definition_port = 14330;
        std::string interface_ip = "0.0.0.0";
    } network;
    
    struct ProtocolConfig {
        int schema_id = 1;
        int schema_version = 13;
        int channel_id = 310;
        int application_id = 18;
        int batch_size = 5;
    } protocol_config;
    
    std::vector<std::shared_ptr<market_core::Instrument>> instruments;
};

ServerConfig load_config(const std::string& config_file)
{
    ServerConfig config;
    
    try {
        std::ifstream file(config_file);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open config file '" << config_file 
                      << "', using defaults\n";
            return config;
        }
        
        json j;
        file >> j;
        
        // Load server config
        if (j.contains("server")) {
            const auto& server = j["server"];
            if (server.contains("protocol")) {
                config.protocol = server["protocol"];
            }
        }
        
        // Load network config
        if (j.contains("network")) {
            const auto& network = j["network"];
            if (network.contains("incremental_feed")) {
                const auto& inc = network["incremental_feed"];
                if (inc.contains("ip")) config.network.incremental_ip = inc["ip"];
                if (inc.contains("port")) config.network.incremental_port = inc["port"];
            }
            if (network.contains("snapshot_feed")) {
                const auto& snap = network["snapshot_feed"];
                if (snap.contains("ip")) config.network.snapshot_ip = snap["ip"];
                if (snap.contains("port")) config.network.snapshot_port = snap["port"];
            }
            if (network.contains("definition_feed")) {
                const auto& def = network["definition_feed"];
                if (def.contains("ip")) config.network.definition_ip = def["ip"];
                if (def.contains("port")) config.network.definition_port = def["port"];
            }
        }
        
        // Load market data config
        if (j.contains("market_data")) {
            const auto& md = j["market_data"];
            if (md.contains("market_mode")) config.market_mode = md["market_mode"];
            if (md.contains("updates_per_second")) config.updates_per_second = md["updates_per_second"];
            if (md.contains("snapshot_interval_seconds")) config.snapshot_interval_seconds = md["snapshot_interval_seconds"];
            if (md.contains("heartbeat_interval_seconds")) config.heartbeat_interval_seconds = md["heartbeat_interval_seconds"];
            if (md.contains("max_book_depth")) config.max_book_depth = md["max_book_depth"];
        }
        
        // Load protocol-specific config
        if (j.contains("protocols") && j["protocols"].contains(config.protocol)) {
            const auto& protocol = j["protocols"][config.protocol];
            if (protocol.contains("schema_id")) config.protocol_config.schema_id = protocol["schema_id"];
            if (protocol.contains("schema_version")) config.protocol_config.schema_version = protocol["schema_version"];
            if (protocol.contains("channel_id")) config.protocol_config.channel_id = protocol["channel_id"];
            if (protocol.contains("application_id")) config.protocol_config.application_id = protocol["application_id"];
            if (protocol.contains("batch_size")) config.protocol_config.batch_size = protocol["batch_size"];
        }
        
        // Load instruments
        if (j.contains("instruments")) {
            for (const auto& inst_json : j["instruments"]) {
                if (!inst_json.contains("enabled") || !inst_json["enabled"]) {
                    continue;
                }
                
                int64_t id = inst_json["instrument_id"];
                std::string symbol = inst_json["symbol"];
                std::string type = inst_json.value("type", "fx_spot");
                
                std::shared_ptr<market_core::Instrument> instrument;
                
                if (type == "fx_spot") {
                    auto fx = std::make_shared<market_core::FXInstrument>(id, symbol);
                    fx->description = inst_json.value("description", symbol);
                    fx->base_currency = inst_json.value("base_currency", "USD");
                    fx->quote_currency = inst_json.value("quote_currency", "EUR");
                    fx->tick_size = inst_json.value("tick_size", 0.00001);
                    fx->contract_size = inst_json.value("contract_size", 100000.0);
                    instrument = fx;
                } else if (type == "futures") {
                    auto futures = std::make_shared<market_core::FuturesInstrument>(id, symbol);
                    futures->description = inst_json.value("description", symbol);
                    futures->underlying = inst_json.value("underlying", "");
                    futures->tick_size = inst_json.value("tick_size", 0.01);
                    futures->multiplier = inst_json.value("multiplier", 1.0);
                    futures->contract_size = inst_json.value("contract_size", 1.0);
                    if (inst_json.contains("maturity_date")) {
                        futures->maturity_date = inst_json["maturity_date"];
                    }
                    if (inst_json.contains("initial_margin")) {
                        futures->initial_margin = inst_json["initial_margin"];
                    }
                    if (inst_json.contains("maintenance_margin")) {
                        futures->maintenance_margin = inst_json["maintenance_margin"];
                    }
                    instrument = futures;
                } else {
                    // Generic instrument
                    instrument = std::make_shared<market_core::Instrument>(id, symbol);
                    instrument->description = inst_json.value("description", symbol);
                }
                
                // Set common properties
                instrument->set_property("initial_price", inst_json.value("initial_price", 1.0));
                instrument->set_property("initial_spread", inst_json.value("initial_spread", 0.001));
                instrument->set_property("price_decimals", static_cast<int64_t>(inst_json.value("price_decimals", 5)));
                
                // Set external ID for protocol mapping
                std::string ext_id_key = config.protocol == "cme" ? "CME_SECURITY_ID" : 
                                       config.protocol == "reuters" ? "REUTERS_RIC" : "UTP_SECURITY_ID";
                instrument->external_ids[ext_id_key] = std::to_string(id);
                
                config.instruments.push_back(instrument);
            }
        }
        
        // Add default instruments if none configured
        if (config.instruments.empty()) {
            auto eurusd = std::make_shared<market_core::FXInstrument>(1, "EURUSD");
            eurusd->description = "EUR/USD Spot FX";
            eurusd->base_currency = "EUR";
            eurusd->quote_currency = "USD";
            eurusd->tick_size = 0.00001;
            eurusd->contract_size = 100000.0;
            eurusd->set_property("initial_price", 1.0850);
            eurusd->set_property("initial_spread", 0.00002);
            eurusd->set_property("price_decimals", int64_t(5));
            config.instruments.push_back(eurusd);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << "\nUsing default configuration\n";
    }
    
    return config;
}

std::shared_ptr<market_protocols::ProtocolAdapter> create_protocol_adapter(
    const std::string& protocol, const ServerConfig::ProtocolConfig& config)
{
    if (protocol == "cme") {
        auto adapter = std::make_shared<cme_protocol::CMEProtocolAdapter>();
        adapter->set_channel_id(config.channel_id);
        adapter->set_batch_size(config.batch_size);
        return adapter;
    } else if (protocol == "reuters") {
        auto adapter = std::make_shared<reuters_protocol::ReutersProtocolAdapter>();
        // Configure Reuters-specific settings
        return adapter;
    } else if (protocol == "utp") {
        auto adapter = std::make_shared<utp_protocol::UTPProtocolAdapter>();
        adapter->set_channel_id(config.channel_id);
        adapter->set_batch_size(config.batch_size);
        return adapter;
    } else {
        throw std::runtime_error("Unsupported protocol: " + protocol);
    }
}

market_core::MarketMode string_to_market_mode(const std::string& mode_str)
{
    if (mode_str == "fast") return market_core::MarketMode::FAST;
    if (mode_str == "volatile") return market_core::MarketMode::VOLATILE;
    if (mode_str == "thin") return market_core::MarketMode::THIN;
    if (mode_str == "trending") return market_core::MarketMode::TRENDING;
    if (mode_str == "stressed") return market_core::MarketMode::STRESSED;
    return market_core::MarketMode::NORMAL;
}

int main(int argc, char* argv[])
{
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Default options
    std::string config_file = "config/unified_server_config.json";
    std::string protocol_override = "";
    std::string mode_override = "";
    int rate_override = -1;
    bool verbose = false;

    // Parse command line arguments
    static struct option long_options[] = {
        { "config", required_argument, 0, 'c' },
        { "protocol", required_argument, 0, 'p' },
        { "mode", required_argument, 0, 'm' },
        { "rate", required_argument, 0, 'r' },
        { "verbose", no_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:p:m:r:vh", long_options, nullptr)) != -1) {
        switch (opt) {
        case 'c':
            config_file = optarg;
            break;
        case 'p':
            protocol_override = optarg;
            break;
        case 'm':
            mode_override = optarg;
            break;
        case 'r':
            rate_override = std::stoi(optarg);
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

    // Load configuration
    ServerConfig config = load_config(config_file);
    
    // Apply command line overrides
    if (!protocol_override.empty()) {
        config.protocol = protocol_override;
    }
    if (!mode_override.empty()) {
        config.market_mode = mode_override;
    }
    if (rate_override > 0) {
        config.updates_per_second = rate_override;
    }

    std::cout << "Unified Market Data Server v2.0.0\n";
    std::cout << "==================================\n";
    std::cout << "Protocol:         " << config.protocol << "\n";
    std::cout << "Market Mode:      " << config.market_mode << "\n";
    std::cout << "Update Rate:      " << config.updates_per_second << " Hz\n";
    std::cout << "Incremental Feed: " << config.network.incremental_ip << ":" << config.network.incremental_port << "\n";
    std::cout << "Snapshot Feed:    " << config.network.snapshot_ip << ":" << config.network.snapshot_port << "\n";
    std::cout << "Instruments:      " << config.instruments.size() << " configured\n\n";

    try {
        // 1. Create core components
        auto book_manager = std::make_shared<market_core::OrderBookManager>();
        auto market_generator = std::make_shared<market_core::MarketDataGenerator>(book_manager);

        // 2. Configure instruments
        for (auto& instrument : config.instruments) {
            book_manager->add_instrument(instrument);
            book_manager->create_order_book(instrument->instrument_id);
            std::cout << "Added instrument: " << instrument->primary_symbol 
                      << " (ID: " << instrument->instrument_id << ")\n";
        }

        // 3. Set up market generator
        market_generator->set_market_mode(string_to_market_mode(config.market_mode));
        auto gen_config = market_generator->get_config();
        gen_config.updates_per_second = config.updates_per_second;
        market_generator->set_config(gen_config);

        // 4. Create protocol adapters and transports
        auto incremental_adapter = create_protocol_adapter(config.protocol, config.protocol_config);
        auto snapshot_adapter = create_protocol_adapter(config.protocol, config.protocol_config);

        auto inc_transport = std::make_shared<market_protocols::UDPMulticastTransport>(
            config.network.incremental_ip, config.network.incremental_port);
        auto snap_transport = std::make_shared<market_protocols::UDPMulticastTransport>(
            config.network.snapshot_ip, config.network.snapshot_port);

        if (!inc_transport->initialize() || !snap_transport->initialize()) {
            std::cerr << "Failed to initialize UDP transports\n";
            return 1;
        }

        incremental_adapter->set_transport(inc_transport);
        snapshot_adapter->set_transport(snap_transport);

        // 5. Create event listeners (if using CME protocol)
        std::shared_ptr<market_core::MarketDataListener> inc_listener, snap_listener;
        
        if (config.protocol == "cme") {
            inc_listener = std::make_shared<cme_protocol::CMEEventListener>(
                book_manager, std::dynamic_pointer_cast<cme_protocol::CMEProtocolAdapter>(incremental_adapter));
            snap_listener = std::make_shared<cme_protocol::CMEEventListener>(
                book_manager, std::dynamic_pointer_cast<cme_protocol::CMEProtocolAdapter>(snapshot_adapter));
        } else {
            // For Reuters and UTP, we'll need to create generic listeners
            // This is a simplified approach - in reality you'd want protocol-specific listeners
            std::cout << "Note: Using simplified event handling for " << config.protocol << " protocol\n";
        }

        if (inc_listener && snap_listener) {
            market_generator->add_listener(inc_listener);
            market_generator->add_listener(snap_listener);
        }

        // 6. Send instrument definitions
        std::cout << "\nSending instrument definitions...\n";
        for (auto& instrument : config.instruments) {
            incremental_adapter->send_instrument_definition(*instrument);
            if (verbose) {
                std::cout << "Sent definition for " << instrument->primary_symbol << "\n";
            }
        }

        // 7. Generate initial snapshots
        std::cout << "Generating initial snapshots...\n";
        for (auto& instrument : config.instruments) {
            auto snapshot = market_generator->generate_snapshot(instrument->instrument_id);
            if (snapshot && verbose) {
                std::cout << "Generated snapshot for " << instrument->primary_symbol << "\n";
            }
        }

        std::cout << "\nStarting market data generation...\n";
        std::cout << "Press Ctrl+C to stop\n\n";

        // 8. Main market data generation loop
        auto last_snapshot = std::chrono::steady_clock::now();
        auto last_heartbeat = std::chrono::steady_clock::now();
        const auto snapshot_interval = std::chrono::seconds(config.snapshot_interval_seconds);
        const auto heartbeat_interval = std::chrono::seconds(config.heartbeat_interval_seconds);
        const auto update_interval = std::chrono::milliseconds(1000 / config.updates_per_second);

        auto stats_timer = std::chrono::steady_clock::now();
        const auto stats_interval = std::chrono::seconds(10);

        while (g_running) {
            auto loop_start = std::chrono::steady_clock::now();

            // Generate market updates
            market_generator->generate_all_instruments();

            // Periodic snapshots
            if (loop_start - last_snapshot >= snapshot_interval) {
                for (auto& instrument : config.instruments) {
                    market_generator->generate_snapshot(instrument->instrument_id);
                }
                last_snapshot = loop_start;
                if (verbose) {
                    std::cout << "Generated periodic snapshots\n";
                }
            }

            // Periodic heartbeats
            if (loop_start - last_heartbeat >= heartbeat_interval) {
                incremental_adapter->send_heartbeat();
                last_heartbeat = loop_start;
                if (verbose) {
                    std::cout << "Sent heartbeat\n";
                }
            }

            // Print statistics
            if (loop_start - stats_timer >= stats_interval) {
                const auto& stats = market_generator->get_statistics();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    loop_start - stats.start_time).count();

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