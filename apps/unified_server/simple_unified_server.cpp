#include "../../protocols/utp/include/utp_simple.h"
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
              << "  -p, --protocol PROTOCOL   Protocol: cme, reuters, utp (default: cme)\n"
              << "  -i, --ip IP              Multicast IP (default: 224.0.28.64)\n"
              << "  -o, --port PORT          Port (default: 14310)\n"
              << "  -r, --rate N             Updates per second (default: 10)\n"
              << "  -v, --verbose            Enable verbose logging\n"
              << "  -h, --help               Show this help message\n\n"
              << "Examples:\n"
              << "  " << program_name << " --protocol utp --ip 224.0.1.100 --port 5000\n"
              << "  " << program_name << " --protocol cme --rate 50\n";
}

struct ServerConfig {
    std::string protocol = "cme";
    std::string ip = "224.0.28.64";
    int port = 14310;
    int updates_per_second = 10;
    bool verbose = false;
};

struct Instrument {
    uint32_t security_id;
    std::string symbol;
    double bid_price;
    double ask_price;
    int64_t bid_size;
    int64_t ask_size;
};

void run_utp_server(const ServerConfig& config)
{
    std::cout << "Starting UTP Server..." << std::endl;

    utp_simple::UTPMessageSender sender(config.ip, config.port);
    if (!sender.initialize()) {
        std::cerr << "Failed to initialize UTP sender" << std::endl;
        return;
    }

    // Sample instruments
    std::vector<Instrument> instruments = {
        { 1, "EURUSD", 1.0850, 1.0852, 1000000, 1000000 },
        { 2, "GBPUSD", 1.2650, 1.2652, 750000, 750000 },
        { 3, "USDJPY", 149.50, 149.52, 1200000, 1200000 }
    };

    // Send security definitions
    for (const auto& instrument : instruments) {
        sender.send_security_definition(instrument.security_id, instrument.symbol);
        if (config.verbose) {
            std::cout << "Sent security definition for " << instrument.symbol << std::endl;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Send initial snapshots
    for (const auto& instrument : instruments) {
        sender.send_snapshot(instrument.security_id, instrument.bid_price, instrument.ask_price,
            instrument.bid_size, instrument.ask_size);
        if (config.verbose) {
            std::cout << "Sent snapshot for " << instrument.symbol << std::endl;
        }
    }

    auto last_heartbeat = std::chrono::steady_clock::now();
    auto last_incremental = std::chrono::steady_clock::now();

    const auto heartbeat_interval = std::chrono::seconds(30);
    const auto update_interval = std::chrono::milliseconds(1000 / config.updates_per_second);

    std::cout << "UTP Server running on " << config.ip << ":" << config.port
              << " at " << config.updates_per_second << " Hz" << std::endl;

    while (g_running) {
        auto now = std::chrono::steady_clock::now();

        // Send heartbeat
        if (now - last_heartbeat >= heartbeat_interval) {
            sender.send_heartbeat();
            if (config.verbose) {
                std::cout << "Sent heartbeat" << std::endl;
            }
            last_heartbeat = now;
        }

        // Send incremental updates
        if (now - last_incremental >= update_interval) {
            for (auto& instrument : instruments) {
                // Simulate small price movements
                double change = (rand() % 21 - 10) * 0.0001; // -10 to +10 pips
                instrument.bid_price += change;
                instrument.ask_price += change;

                // Ensure spread is maintained
                if (instrument.ask_price <= instrument.bid_price) {
                    instrument.ask_price = instrument.bid_price + 0.0002;
                }

                sender.send_incremental_update(instrument.security_id,
                    instrument.bid_price, instrument.ask_price,
                    instrument.bid_size, instrument.ask_size);

                if (config.verbose) {
                    std::cout << "Sent update for " << instrument.symbol
                              << " bid=" << instrument.bid_price
                              << " ask=" << instrument.ask_price << std::endl;
                }
            }
            last_incremental = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void run_cme_server(const ServerConfig& config)
{
    std::cout << "Starting CME MDP 3.0 Server..." << std::endl;
    std::cout << "CME Server configured for " << config.ip << ":" << config.port
              << " at " << config.updates_per_second << " Hz" << std::endl;
    std::cout << "Use the dedicated cme_server executable for full CME MDP 3.0 implementation" << std::endl;
    std::cout << "Command: ./cme_server config/server_config.json" << std::endl;

    while (g_running) {
        if (config.verbose) {
            std::cout << "CME server placeholder running..." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

void run_reuters_server(const ServerConfig& config)
{
    std::cout << "Starting Reuters FIX/SBE Server..." << std::endl;
    std::cout << "Reuters Server configured for " << config.ip << ":" << config.port
              << " at " << config.updates_per_second << " Hz" << std::endl;
    std::cout << "Use the dedicated reuters_server executable for full Reuters implementation" << std::endl;
    std::cout << "Command: ./reuters_server config/server_config.json" << std::endl;

    while (g_running) {
        if (config.verbose) {
            std::cout << "Reuters server placeholder running..." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

int main(int argc, char* argv[])
{
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    ServerConfig config;

    // Parse command line arguments
    static struct option long_options[] = {
        { "protocol", required_argument, 0, 'p' },
        { "ip", required_argument, 0, 'i' },
        { "port", required_argument, 0, 'o' },
        { "rate", required_argument, 0, 'r' },
        { "verbose", no_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "p:i:o:r:vh", long_options, nullptr)) != -1) {
        switch (opt) {
        case 'p':
            config.protocol = optarg;
            break;
        case 'i':
            config.ip = optarg;
            break;
        case 'o':
            config.port = std::stoi(optarg);
            break;
        case 'r':
            config.updates_per_second = std::stoi(optarg);
            break;
        case 'v':
            config.verbose = true;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    std::cout << "=== Unified Market Data Server ===" << std::endl;
    std::cout << "Protocol: " << config.protocol << std::endl;
    std::cout << "Address: " << config.ip << ":" << config.port << std::endl;
    std::cout << "Rate: " << config.updates_per_second << " Hz" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl
              << std::endl;

    try {
        if (config.protocol == "utp") {
            run_utp_server(config);
        } else if (config.protocol == "cme") {
            run_cme_server(config);
        } else if (config.protocol == "reuters") {
            run_reuters_server(config);
        } else {
            std::cerr << "Unsupported protocol: " << config.protocol << std::endl;
            std::cerr << "Supported protocols: utp, cme, reuters" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Unified server shutdown complete." << std::endl;
    return 0;
}