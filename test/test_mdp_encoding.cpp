#include "core/order_book.h"
#include "messages/mdp_messages.h"
#include "messages/message_factory.h"
#include "messages/sbe_encoder.h"
#include "utils/logger.h"
#include <iomanip>
#include <iostream>

void print_hex(const std::vector<uint8_t>& data, const std::string& label)
{
    std::cout << "\n"
              << label << " (" << data.size() << " bytes):" << std::endl;

    for (size_t i = 0; i < data.size(); ++i) {
        if (i % 16 == 0) {
            if (i > 0)
                std::cout << std::endl;
            std::cout << std::setfill('0') << std::setw(4) << std::hex << i << ": ";
        }
        std::cout << std::setfill('0') << std::setw(2) << std::hex
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

int main()
{
    std::cout << "Testing MDP 3.0 Message Encoding\n"
              << std::endl;

    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);

    // Test 1: Encode Admin Messages
    std::cout << "=== Test 1: Admin Messages ===" << std::endl;

    auto heartbeat = cme_mock::MDPMessageEncoder::encode_heartbeat(
        cme_mock::get_timestamp_ns());
    print_hex(heartbeat, "Heartbeat Message");

    auto channel_reset = cme_mock::MDPMessageEncoder::encode_channel_reset(
        cme_mock::get_timestamp_ns());
    print_hex(channel_reset, "Channel Reset Message");

    // Test 2: Create and encode a snapshot
    std::cout << "\n=== Test 2: Snapshot Message ===" << std::endl;

    // Create an order book with some data
    cme_mock::OrderBook book(1, "ES");
    book.add_bid(4500.00, 100);
    book.add_bid(4499.75, 200);
    book.add_bid(4499.50, 300);
    book.add_ask(4500.25, 100);
    book.add_ask(4500.50, 200);
    book.add_ask(4500.75, 300);
    book.add_trade(4500.25, 50, 'B');

    // Create snapshot message
    auto snapshot = cme_mock::create_snapshot_from_book(book, 1000, 1);
    std::cout << "Snapshot contains " << snapshot.entries.size() << " entries" << std::endl;

    // Encode snapshot
    auto encoded_snapshot = cme_mock::MDPMessageEncoder::encode_snapshot_full_refresh(snapshot);
    print_hex(encoded_snapshot, "Encoded Snapshot");

    // Test 3: Incremental Updates
    std::cout << "\n=== Test 3: Incremental Updates ===" << std::endl;

    // Price level update
    auto price_update = cme_mock::create_price_level_update(
        1, cme_mock::MDUpdateAction::Change, cme_mock::MDEntryType::Bid,
        4500.25, 150, 1, 2);

    auto encoded_price = cme_mock::MDPMessageEncoder::encode_incremental_refresh(price_update);
    print_hex(encoded_price, "Price Level Update");

    // Trade update
    auto trade_update = cme_mock::create_trade_update(
        1, 4500.50, 75, 'S', 3);

    auto encoded_trade = cme_mock::MDPMessageEncoder::encode_incremental_refresh(trade_update);
    print_hex(encoded_trade, "Trade Update");

    // Test 4: Batch Message
    std::cout << "\n=== Test 4: Batch Incremental Message ===" << std::endl;

    cme_mock::IncrementalRefresh batch;
    batch.header.sequence_number = 0;
    batch.header.sending_time = cme_mock::get_timestamp_ns();
    batch.header.msg_count = 3;
    batch.transact_time = cme_mock::get_timestamp_ns();

    // Add multiple price levels
    for (int i = 0; i < 3; ++i) {
        cme_mock::MDPriceLevel level;
        level.update_action = cme_mock::MDUpdateAction::New;
        level.entry_type = cme_mock::MDEntryType::Bid;
        level.security_id = 1;
        level.rpt_seq = 10 + i;
        level.price = static_cast<int64_t>((4499.00 - i * 0.25) * 100);
        level.quantity = 100 * (i + 1);
        level.number_of_orders = i + 1;
        level.price_level = i + 1;
        batch.price_levels.push_back(level);
    }

    // Add a trade
    cme_mock::MDTrade trade;
    trade.security_id = 1;
    trade.rpt_seq = 13;
    trade.price = static_cast<int64_t>(4500.00 * 100);
    trade.quantity = 25;
    trade.number_of_orders = 1;
    trade.aggressor_side = 'B';
    batch.trades.push_back(trade);

    auto encoded_batch = cme_mock::MDPMessageEncoder::encode_incremental_refresh(batch);
    print_hex(encoded_batch, "Batch Incremental Message");

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Heartbeat size: " << heartbeat.size() << " bytes" << std::endl;
    std::cout << "Channel Reset size: " << channel_reset.size() << " bytes" << std::endl;
    std::cout << "Snapshot size: " << encoded_snapshot.size() << " bytes" << std::endl;
    std::cout << "Price Update size: " << encoded_price.size() << " bytes" << std::endl;
    std::cout << "Trade Update size: " << encoded_trade.size() << " bytes" << std::endl;
    std::cout << "Batch Message size: " << encoded_batch.size() << " bytes" << std::endl;

    return 0;
}