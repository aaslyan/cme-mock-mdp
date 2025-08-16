#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <signal.h>
#include <thread>

#include "core/order_book.h"
#include "messages/sbe_decoder.h"
#include "messages/sbe_encoder.h"
#include "network/udp_receiver.h"
#include "utils/logger.h"

std::atomic<bool> g_running { true };

void signal_handler(int signal)
{
    if (signal == SIGINT) {
        std::cout << "\nShutdown signal received..." << std::endl;
        g_running = false;
    }
}

class CMETestClient {
public:
    CMETestClient(const std::string& snapshot_ip, uint16_t snapshot_port,
        const std::string& incremental_ip, uint16_t incremental_port)
        : snapshot_receiver_(snapshot_ip, snapshot_port)
        , incremental_receiver_(incremental_ip, incremental_port)
    {
    }

    bool initialize()
    {
        if (!snapshot_receiver_.initialize()) {
            LOG_ERROR("Failed to initialize snapshot receiver");
            return false;
        }

        if (!incremental_receiver_.initialize()) {
            LOG_ERROR("Failed to initialize incremental receiver");
            return false;
        }

        // Set up message callbacks
        snapshot_receiver_.set_message_callback(
            [this](const std::vector<uint8_t>& data) {
                handle_snapshot_message(data);
            });

        incremental_receiver_.set_message_callback(
            [this](const std::vector<uint8_t>& data) {
                handle_incremental_message(data);
            });

        return true;
    }

    void run()
    {
        std::cout << "CME Test Client started" << std::endl;
        std::cout << "Waiting for market data..." << std::endl;

        // Start receiver threads
        std::thread snapshot_thread([this]() {
            while (g_running) {
                snapshot_receiver_.receive_messages(1000);
            }
        });

        std::thread incremental_thread([this]() {
            while (g_running) {
                incremental_receiver_.receive_messages(1000);
            }
        });

        // Statistics thread
        std::thread stats_thread([this]() {
            auto last_time = std::chrono::steady_clock::now();

            while (g_running) {
                std::this_thread::sleep_for(std::chrono::seconds(5));

                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_time).count();

                if (elapsed >= 5) {
                    print_statistics();
                    print_order_books();
                    last_time = now;
                }
            }
        });

        // Wait for shutdown
        snapshot_thread.join();
        incremental_thread.join();
        stats_thread.join();
    }

private:
    cme_mock::UDPReceiver snapshot_receiver_;
    cme_mock::UDPReceiver incremental_receiver_;

    // Order books by security ID
    std::map<uint32_t, std::shared_ptr<cme_mock::OrderBook>> order_books_;

    // Statistics
    uint64_t snapshots_received_ = 0;
    uint64_t incrementals_received_ = 0;
    uint64_t trades_received_ = 0;
    uint64_t book_updates_received_ = 0;
    uint32_t last_snapshot_seq_ = 0;
    uint32_t last_incremental_seq_ = 0;

    void handle_snapshot_message(const std::vector<uint8_t>& data)
    {
        std::cout << "\n[RAW MESSAGE] Snapshot received, size: " << data.size() << " bytes" << std::endl;

        cme_mock::SBEDecoder decoder(data);

        // Decode packet header
        uint32_t seq_num;
        uint64_t sending_time;
        if (!cme_mock::MDPMessageDecoder::decode_packet_header(decoder, seq_num, sending_time)) {
            LOG_WARNING("Failed to decode packet header");
            std::cout << "Failed to decode packet header" << std::endl;
            return;
        }

        // Decode message header
        uint16_t block_length, schema_id, version;
        uint16_t template_id = cme_mock::MDPMessageDecoder::decode_message_header(
            decoder, block_length, schema_id, version);

        std::cout << "[HEADER] Seq: " << seq_num << ", Template: " << template_id
                  << ", Schema: " << schema_id << ", Version: " << version
                  << ", Block: " << block_length << std::endl;

        if (template_id == cme_mock::TEMPLATE_SNAPSHOT_FULL_REFRESH) {
            cme_mock::SnapshotFullRefresh snapshot;
            snapshot.header.sequence_number = seq_num;
            snapshot.header.sending_time = sending_time;

            if (cme_mock::MDPMessageDecoder::decode_snapshot_full_refresh(decoder, snapshot)) {
                process_snapshot(snapshot);
                snapshots_received_++;
                last_snapshot_seq_ = seq_num;
            } else {
                std::cout << "Failed to decode snapshot message" << std::endl;
            }
        } else {
            std::cout << "Unknown template ID: " << template_id << std::endl;
        }
    }

    void handle_incremental_message(const std::vector<uint8_t>& data)
    {
        std::cout << "\n[RAW MESSAGE] Incremental received, size: " << data.size() << " bytes" << std::endl;

        cme_mock::SBEDecoder decoder(data);

        // Decode packet header
        uint32_t seq_num;
        uint64_t sending_time;
        if (!cme_mock::MDPMessageDecoder::decode_packet_header(decoder, seq_num, sending_time)) {
            std::cout << "Failed to decode packet header" << std::endl;
            return;
        }

        // Decode message header
        uint16_t block_length, schema_id, version;
        uint16_t template_id = cme_mock::MDPMessageDecoder::decode_message_header(
            decoder, block_length, schema_id, version);

        std::cout << "[HEADER] Seq: " << seq_num << ", Template: " << template_id
                  << ", Schema: " << schema_id << ", Version: " << version
                  << ", Block: " << block_length << std::endl;

        if (template_id == cme_mock::TEMPLATE_INCREMENTAL_REFRESH_BOOK) {
            cme_mock::IncrementalRefresh incremental;
            incremental.header.sequence_number = seq_num;
            incremental.header.sending_time = sending_time;

            if (cme_mock::MDPMessageDecoder::decode_incremental_refresh(decoder, incremental)) {
                process_incremental(incremental);
                incrementals_received_++;
                last_incremental_seq_ = seq_num;
            } else {
                std::cout << "Failed to decode incremental message" << std::endl;
            }
        } else {
            std::cout << "Unknown template ID: " << template_id << std::endl;
        }
    }

    void process_snapshot(const cme_mock::SnapshotFullRefresh& snapshot)
    {
        uint32_t security_id = snapshot.security_id;

        // Enhanced logging for snapshot
        std::cout << "\n=== SNAPSHOT RECEIVED ===" << std::endl;
        std::cout << "Security ID: " << security_id << std::endl;
        std::cout << "Seq Number: " << snapshot.rpt_seq << std::endl;
        std::cout << "Last Msg Seq Num: " << snapshot.last_msg_seq_num_processed << std::endl;
        std::cout << "Entries: " << snapshot.entries.size() << std::endl;

        // Create or get order book
        auto it = order_books_.find(security_id);
        if (it == order_books_.end()) {
            std::string symbol = "SEC" + std::to_string(security_id);
            order_books_[security_id] = std::make_shared<cme_mock::OrderBook>(security_id, symbol);
            std::cout << "Created new order book for security " << security_id << std::endl;
        }

        auto& book = order_books_[security_id];
        book->clear();

        // Apply snapshot entries with detailed logging
        std::cout << "Order Book Entries:" << std::endl;
        for (size_t i = 0; i < snapshot.entries.size(); ++i) {
            const auto& entry = snapshot.entries[i];
            double price = entry.price / 100.0; // Convert from scaled

            std::cout << "  [" << i << "] Type: "
                      << (entry.entry_type == cme_mock::MDEntryType::Bid ? "BID" : entry.entry_type == cme_mock::MDEntryType::Offer ? "ASK"
                                 : entry.entry_type == cme_mock::MDEntryType::Trade                                                 ? "TRADE"
                                                                                                                                    : "OTHER")
                      << ", Price: " << std::fixed << std::setprecision(5) << price
                      << ", Qty: " << entry.quantity
                      << ", Level: " << (int)entry.price_level << std::endl;

            switch (entry.entry_type) {
            case cme_mock::MDEntryType::Bid:
                book->add_bid(price, entry.quantity);
                break;
            case cme_mock::MDEntryType::Offer:
                book->add_ask(price, entry.quantity);
                break;
            case cme_mock::MDEntryType::Trade:
                // Trade info for display
                break;
            default:
                break;
            }
        }
        std::cout << "========================\n"
                  << std::endl;
    }

    void process_incremental(const cme_mock::IncrementalRefresh& incremental)
    {
        std::cout << "\n--- INCREMENTAL UPDATE ---" << std::endl;
        std::cout << "Price Levels: " << incremental.price_levels.size()
                  << ", Trades: " << incremental.trades.size() << std::endl;

        // Process price levels
        for (size_t i = 0; i < incremental.price_levels.size(); ++i) {
            const auto& level = incremental.price_levels[i];
            auto it = order_books_.find(level.security_id);
            if (it == order_books_.end()) {
                std::cout << "  Unknown security ID: " << level.security_id << std::endl;
                continue;
            }

            auto& book = it->second;
            double price = level.price / 100.0;

            std::cout << "  [" << i << "] Security: " << level.security_id
                      << ", Action: " << (level.update_action == cme_mock::MDUpdateAction::New ? "NEW" : level.update_action == cme_mock::MDUpdateAction::Change ? "CHANGE"
                                                                                                                                                                 : "DELETE")
                      << ", Type: " << (level.entry_type == cme_mock::MDEntryType::Bid ? "BID" : level.entry_type == cme_mock::MDEntryType::Offer ? "ASK"
                                                                                                                                                  : "OTHER")
                      << ", Price: " << std::fixed << std::setprecision(5) << price
                      << ", Qty: " << level.quantity << std::endl;

            switch (level.entry_type) {
            case cme_mock::MDEntryType::Bid:
                if (level.update_action == cme_mock::MDUpdateAction::Delete) {
                    book->remove_bid(price);
                } else {
                    book->update_bid(price, level.quantity);
                }
                break;
            case cme_mock::MDEntryType::Offer:
                if (level.update_action == cme_mock::MDUpdateAction::Delete) {
                    book->remove_ask(price);
                } else {
                    book->update_ask(price, level.quantity);
                }
                break;
            default:
                break;
            }

            book_updates_received_++;
        }

        // Process trades
        for (size_t i = 0; i < incremental.trades.size(); ++i) {
            const auto& trade = incremental.trades[i];
            auto it = order_books_.find(trade.security_id);
            if (it == order_books_.end()) {
                std::cout << "  Trade for unknown security ID: " << trade.security_id << std::endl;
                continue;
            }

            auto& book = it->second;
            double price = trade.price / 100.0;

            std::cout << "  TRADE [" << i << "] Security: " << trade.security_id
                      << ", Price: " << std::fixed << std::setprecision(5) << price
                      << ", Qty: " << trade.quantity
                      << ", Side: " << (trade.aggressor_side == 'B' ? "BUY" : "SELL")
                      << std::endl;

            book->add_trade(price, trade.quantity, trade.aggressor_side);
            trades_received_++;
        }

        if (!incremental.price_levels.empty() || !incremental.trades.empty()) {
            std::cout << "-------------------------\n"
                      << std::endl;
        }
    }

    void print_statistics()
    {
        std::cout << "\n=== Statistics ===" << std::endl;
        std::cout << "Snapshots received: " << snapshots_received_
                  << " (last seq: " << last_snapshot_seq_ << ")" << std::endl;
        std::cout << "Incrementals received: " << incrementals_received_
                  << " (last seq: " << last_incremental_seq_ << ")" << std::endl;
        std::cout << "Book updates: " << book_updates_received_ << std::endl;
        std::cout << "Trades: " << trades_received_ << std::endl;
        std::cout << "Order books: " << order_books_.size() << std::endl;

        std::cout << "Snapshot receiver - Messages: " << snapshot_receiver_.get_messages_received()
                  << ", Bytes: " << snapshot_receiver_.get_bytes_received() << std::endl;
        std::cout << "Incremental receiver - Messages: " << incremental_receiver_.get_messages_received()
                  << ", Bytes: " << incremental_receiver_.get_bytes_received() << std::endl;
    }

    void print_order_books()
    {
        std::cout << "\n=== Order Books ===" << std::endl;

        for (const auto& [security_id, book] : order_books_) {
            auto bids = book->get_bids(3);
            auto asks = book->get_asks(3);
            const auto& stats = book->get_stats();

            std::cout << "\n"
                      << book->get_symbol() << " (ID: " << security_id << ")" << std::endl;

            // Print asks (reverse order)
            for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
                std::cout << "      " << std::setw(8) << it->quantity
                          << " @ " << std::fixed << std::setprecision(2) << it->price << " ASK" << std::endl;
            }

            // Print spread
            std::cout << "      ----------- Spread: " << std::fixed << std::setprecision(2)
                      << book->get_spread() << " -----------" << std::endl;

            // Print bids
            for (const auto& bid : bids) {
                std::cout << "BID " << std::fixed << std::setprecision(2) << bid.price
                          << " @ " << std::setw(8) << bid.quantity << std::endl;
            }

            // Print last trade
            if (stats.last_price > 0) {
                std::cout << "Last: " << std::fixed << std::setprecision(2) << stats.last_price
                          << " | Volume: " << stats.total_volume
                          << " | Trades: " << stats.trade_count << std::endl;
            }
        }
    }
};

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);

    std::cout << "CME Test Client v1.0" << std::endl;
    std::cout << "====================" << std::endl;

    // Default ports
    std::string snapshot_ip = "127.0.0.1";
    uint16_t snapshot_port = 14320;
    std::string incremental_ip = "127.0.0.1";
    uint16_t incremental_port = 14310;

    // Parse command line arguments
    if (argc >= 3) {
        snapshot_port = std::atoi(argv[1]);
        incremental_port = std::atoi(argv[2]);
    }

    std::cout << "Connecting to:" << std::endl;
    std::cout << "  Snapshot feed: " << snapshot_ip << ":" << snapshot_port << std::endl;
    std::cout << "  Incremental feed: " << incremental_ip << ":" << incremental_port << std::endl;

    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);

    try {
        CMETestClient client(snapshot_ip, snapshot_port, incremental_ip, incremental_port);

        if (!client.initialize()) {
            std::cerr << "Failed to initialize client" << std::endl;
            return 1;
        }

        client.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Client stopped successfully" << std::endl;
    return 0;
}