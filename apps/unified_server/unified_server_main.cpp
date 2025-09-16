#include "../../core/include/instrument.h"
#include "../../core/include/market_data_generator.h"
#include "../../core/include/order_book_manager.h"
#include "../../protocols/cme/include/cme_event_listener.h"
#include "../../protocols/cme/include/cme_protocol_adapter.h"
#include "../../protocols/common/include/tcp_transport.h"
#include "../../protocols/common/include/udp_multicast_transport.h"
#include "../../protocols/common/include/udp_transport.h"
#include "../../protocols/reuters/include/reuters_protocol_adapter.h"
#include "../../protocols/utp/include/utp_protocol_adapter.h"

#include "native_json.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <signal.h>
#include <thread>
#include <vector>

using json = native_json::JsonValue;

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

        auto j = native_json::parse_file(config_file);

        // Load server config
        if (j->contains("server")) {
            const auto server = j->operator[]("server");
            if (server && server->contains("protocol")) {
                config.protocol = server->operator[]("protocol")->as_string();
            }
        }

        // Load network config
        if (j->contains("network")) {
            const auto network = j->operator[]("network");
            if (network && network->contains("incremental_feed")) {
                const auto inc = network->operator[]("incremental_feed");
                if (inc && inc->contains("ip"))
                    config.network.incremental_ip = inc->operator[]("ip")->as_string();
                if (inc && inc->contains("port"))
                    config.network.incremental_port = static_cast<int>(*inc->operator[]("port"));
            }
            if (network && network->contains("snapshot_feed")) {
                const auto snap = network->operator[]("snapshot_feed");
                if (snap && snap->contains("ip"))
                    config.network.snapshot_ip = snap->operator[]("ip")->as_string();
                if (snap && snap->contains("port"))
                    config.network.snapshot_port = static_cast<int>(*snap->operator[]("port"));
            }
            if (network && network->contains("definition_feed")) {
                const auto def = network->operator[]("definition_feed");
                if (def && def->contains("ip"))
                    config.network.definition_ip = def->operator[]("ip")->as_string();
                if (def && def->contains("port"))
                    config.network.definition_port = static_cast<int>(*def->operator[]("port"));
            }
        }

        // Load market data config
        if (j->contains("market_data")) {
            const auto md = j->operator[]("market_data");
            if (md && md->contains("market_mode"))
                config.market_mode = md->operator[]("market_mode")->as_string();
            if (md && md->contains("updates_per_second"))
                config.updates_per_second = static_cast<int>(*md->operator[]("updates_per_second"));
            if (md && md->contains("snapshot_interval_seconds"))
                config.snapshot_interval_seconds = static_cast<int>(*md->operator[]("snapshot_interval_seconds"));
            if (md && md->contains("heartbeat_interval_seconds"))
                config.heartbeat_interval_seconds = static_cast<int>(*md->operator[]("heartbeat_interval_seconds"));
            if (md && md->contains("max_book_depth"))
                config.max_book_depth = static_cast<int>(*md->operator[]("max_book_depth"));
        }

        // Load protocol-specific config
        if (j->contains("protocols")) {
            const auto protocols = j->operator[]("protocols");
            if (protocols && protocols->contains(config.protocol)) {
                const auto protocol = protocols->operator[](config.protocol);
                if (protocol && protocol->contains("schema_id"))
                    config.protocol_config.schema_id = static_cast<int>(*protocol->operator[]("schema_id"));
                if (protocol && protocol->contains("schema_version"))
                    config.protocol_config.schema_version = static_cast<int>(*protocol->operator[]("schema_version"));
                if (protocol && protocol->contains("channel_id"))
                    config.protocol_config.channel_id = static_cast<int>(*protocol->operator[]("channel_id"));
                if (protocol && protocol->contains("application_id"))
                    config.protocol_config.application_id = static_cast<int>(*protocol->operator[]("application_id"));
                if (protocol && protocol->contains("batch_size"))
                    config.protocol_config.batch_size = static_cast<int>(*protocol->operator[]("batch_size"));
            }
        }

        // Load instruments
        if (j->contains("instruments")) {
            const auto instruments_array = j->operator[]("instruments");
            if (instruments_array && instruments_array->is_array()) {
                const auto& arr = instruments_array->as_array();
                for (const auto& inst_ptr : arr) {
                    if (!inst_ptr)
                        continue;

                    bool enabled = true;
                    if (inst_ptr->contains("enabled")) {
                        enabled = static_cast<bool>(*inst_ptr->operator[]("enabled"));
                    }
                    if (!enabled)
                        continue;

                    int64_t id = static_cast<long>(*inst_ptr->operator[]("instrument_id"));
                    std::string symbol = inst_ptr->operator[]("symbol")->as_string();
                    std::string type = "fx_spot";
                    if (inst_ptr->contains("type")) {
                        type = inst_ptr->operator[]("type")->as_string();
                    }

                    std::shared_ptr<market_core::Instrument> instrument;

                    if (type == "fx_spot") {
                        auto fx = std::make_shared<market_core::FXSpotInstrument>(id, symbol);
                        fx->description = inst_ptr->contains("description") ? inst_ptr->operator[]("description")->as_string() : symbol;
                        fx->base_currency = inst_ptr->contains("base_currency") ? inst_ptr->operator[]("base_currency")->as_string() : "USD";
                        fx->quote_currency = inst_ptr->contains("quote_currency") ? inst_ptr->operator[]("quote_currency")->as_string() : "EUR";
                        fx->tick_size = inst_ptr->contains("tick_size") ? static_cast<double>(*inst_ptr->operator[]("tick_size")) : 0.00001;
                        fx->standard_lot_size = inst_ptr->contains("contract_size") ? static_cast<double>(*inst_ptr->operator[]("contract_size")) : 100000.0;
                        instrument = fx;
                    } else if (type == "futures") {
                        auto futures = std::make_shared<market_core::FuturesInstrument>(id, symbol);
                        futures->description = inst_ptr->contains("description") ? inst_ptr->operator[]("description")->as_string() : symbol;
                        futures->underlying = inst_ptr->contains("underlying") ? inst_ptr->operator[]("underlying")->as_string() : "";
                        futures->tick_size = inst_ptr->contains("tick_size") ? static_cast<double>(*inst_ptr->operator[]("tick_size")) : 0.01;
                        futures->multiplier = inst_ptr->contains("multiplier") ? static_cast<double>(*inst_ptr->operator[]("multiplier")) : 1.0;
                        futures->contract_size = inst_ptr->contains("contract_size") ? static_cast<double>(*inst_ptr->operator[]("contract_size")) : 1.0;
                        if (inst_ptr->contains("maturity_date")) {
                            futures->maturity_date = inst_ptr->operator[]("maturity_date")->as_string();
                        }
                        if (inst_ptr->contains("initial_margin")) {
                            futures->initial_margin = static_cast<double>(*inst_ptr->operator[]("initial_margin"));
                        }
                        if (inst_ptr->contains("maintenance_margin")) {
                            futures->maintenance_margin = static_cast<double>(*inst_ptr->operator[]("maintenance_margin"));
                        }
                        instrument = futures;
                    } else {
                        // Generic instrument
                        instrument = std::make_shared<market_core::Instrument>(id, symbol, market_core::InstrumentType::UNKNOWN);
                        instrument->description = inst_ptr->contains("description") ? inst_ptr->operator[]("description")->as_string() : symbol;
                    }

                    // Set common properties
                    double initial_price = inst_ptr->contains("initial_price") ? static_cast<double>(*inst_ptr->operator[]("initial_price")) : 1.0;
                    double initial_spread = inst_ptr->contains("initial_spread") ? static_cast<double>(*inst_ptr->operator[]("initial_spread")) : 0.001;
                    int64_t price_decimals = inst_ptr->contains("price_decimals") ? static_cast<long>(*inst_ptr->operator[]("price_decimals")) : 5;

                    instrument->set_property("initial_price", initial_price);
                    instrument->set_property("initial_spread", initial_spread);
                    instrument->set_property("price_decimals", price_decimals);

                    // Set external ID for protocol mapping
                    std::string ext_id_key = config.protocol == "cme" ? "CME_SECURITY_ID" : config.protocol == "reuters" ? "REUTERS_RIC"
                                                                                                                         : "UTP_SECURITY_ID";
                    instrument->external_ids[ext_id_key] = std::to_string(id);

                    config.instruments.push_back(instrument);
                }
            }
        }

        // Add default instruments if none configured
        if (config.instruments.empty()) {
            auto eurusd = std::make_shared<market_core::FXSpotInstrument>(1, "EURUSD");
            eurusd->description = "EUR/USD Spot FX";
            eurusd->base_currency = "EUR";
            eurusd->quote_currency = "USD";
            eurusd->tick_size = 0.00001;
            eurusd->standard_lot_size = 100000.0;
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

std::shared_ptr<market_protocols::IProtocolAdapter> create_protocol_adapter(
    const std::string& protocol, const ServerConfig::ProtocolConfig& config)
{
    if (protocol == "cme") {
        auto adapter = std::make_shared<cme_protocol::CMEProtocolAdapter>();
        // CME configuration would be set during initialization
        return adapter;
    } else {
        throw std::runtime_error("Only CME protocol is currently supported in unified server. Protocol requested: " + protocol);
    }
}

market_core::MarketMode string_to_market_mode(const std::string& mode_str)
{
    if (mode_str == "fast")
        return market_core::MarketMode::FAST;
    if (mode_str == "volatile")
        return market_core::MarketMode::VOLATILE;
    if (mode_str == "thin")
        return market_core::MarketMode::THIN;
    if (mode_str == "trending")
        return market_core::MarketMode::TRENDING;
    if (mode_str == "stressed")
        return market_core::MarketMode::STRESSED;
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

        auto inc_transport = std::make_shared<market_protocols::UDPTransport>(
            config.network.incremental_ip, config.network.incremental_port);
        auto snap_transport = std::make_shared<market_protocols::UDPTransport>(
            config.network.snapshot_ip, config.network.snapshot_port);

        if (!inc_transport->initialize() || !snap_transport->initialize()) {
            std::cerr << "Failed to initialize UDP transports\n";
            return 1;
        }

        incremental_adapter->set_transport(inc_transport);
        snapshot_adapter->set_transport(snap_transport);

        // 5. Create event listeners (if using CME protocol)
        std::shared_ptr<market_core::IMarketEventListener> inc_listener, snap_listener;

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