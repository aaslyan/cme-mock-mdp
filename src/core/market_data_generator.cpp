#include "core/market_data_generator.h"
#include "messages/message_factory.h"
#include "utils/logger.h"
#include <algorithm>
#include <sstream>

namespace cme_mock {

MarketDataGenerator::MarketDataGenerator(
    std::shared_ptr<OrderBookManager> book_manager,
    std::shared_ptr<IncrementalFeedPublisher> inc_publisher)
    : book_manager_(book_manager)
    , inc_publisher_(inc_publisher)
{

    // Initialize random number generator with current time
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng_.seed(static_cast<unsigned int>(seed));
}

void MarketDataGenerator::generate_update()
{
    auto security_ids = book_manager_->get_all_security_ids();
    if (security_ids.empty())
        return;

    // Randomly select a security to update
    std::uniform_int_distribution<> security_dist(0, security_ids.size() - 1);
    uint32_t security_id = security_ids[security_dist(rng_)];

    // Decide whether to generate a trade or book update
    if (should_generate_trade()) {
        generate_trade(security_id);
    } else {
        generate_book_update(security_id);
    }

    updates_generated_++;
}

void MarketDataGenerator::generate_batch(int count)
{
    IncrementalRefresh batch;
    batch.transact_time = get_timestamp_ns();

    for (int i = 0; i < count; ++i) {
        auto security_ids = book_manager_->get_all_security_ids();
        if (security_ids.empty())
            continue;

        std::uniform_int_distribution<> security_dist(0, security_ids.size() - 1);
        uint32_t security_id = security_ids[security_dist(rng_)];

        auto book = book_manager_->get_order_book(security_id);
        if (!book)
            continue;

        if (should_generate_trade() && book->get_best_bid() > 0 && book->get_best_ask() > 0) {
            // Generate trade at mid or near touch
            double bid = book->get_best_bid();
            double ask = book->get_best_ask();
            double trade_price = uniform_dist_(rng_) < 0.5 ? bid : ask;
            int32_t quantity = calculate_quantity();
            char aggressor = trade_price == ask ? 'B' : 'S';

            MDTrade trade;
            trade.security_id = security_id;
            trade.rpt_seq = rpt_seq_++;
            trade.price = static_cast<int64_t>(trade_price * 100);
            trade.quantity = quantity;
            trade.number_of_orders = 1;
            trade.aggressor_side = aggressor;

            batch.trades.push_back(trade);

            // Update the book
            book->add_trade(trade_price, quantity, aggressor);

            // Remove or reduce touched level
            if (aggressor == 'B') {
                auto asks = book->get_asks(1);
                if (!asks.empty()) {
                    int64_t remaining = static_cast<int64_t>(asks[0].quantity) - quantity;
                    book->update_ask(trade_price, remaining > 0 ? remaining : 0);
                }
            } else {
                auto bids = book->get_bids(1);
                if (!bids.empty()) {
                    int64_t remaining = static_cast<int64_t>(bids[0].quantity) - quantity;
                    book->update_bid(trade_price, remaining > 0 ? remaining : 0);
                }
            }

            trades_generated_++;
        } else {
            // Generate book update
            bool is_bid = uniform_dist_(rng_) < 0.5;
            double base_price = is_bid ? book->get_best_bid() : book->get_best_ask();
            if (base_price <= 0)
                continue;

            // Determine price level to update (1-5)
            std::uniform_int_distribution<> level_dist(1, 5);
            uint8_t level = level_dist(rng_);

            double tick_size = 0.25; // TODO: Get from instrument config
            double price = is_bid ? base_price - (level - 1) * tick_size : base_price + (level - 1) * tick_size;

            int32_t quantity = calculate_quantity();
            MDUpdateAction action = quantity > 0 ? MDUpdateAction::Change : MDUpdateAction::Delete;

            MDPriceLevel price_level;
            price_level.update_action = action;
            price_level.entry_type = is_bid ? MDEntryType::Bid : MDEntryType::Offer;
            price_level.security_id = security_id;
            price_level.rpt_seq = rpt_seq_++;
            price_level.price = static_cast<int64_t>(price * 100);
            price_level.quantity = quantity;
            price_level.number_of_orders = quantity > 0 ? (quantity / 100 + 1) : 0;
            price_level.price_level = level;

            batch.price_levels.push_back(price_level);

            // Update the book
            if (is_bid) {
                book->update_bid(price, quantity);
            } else {
                book->update_ask(price, quantity);
            }
        }
    }

    if (!batch.price_levels.empty() || !batch.trades.empty()) {
        inc_publisher_->publish_batch(batch);
        updates_generated_ += count;
    }
}

void MarketDataGenerator::generate_book_update(uint32_t security_id)
{
    auto book = book_manager_->get_order_book(security_id);
    if (!book)
        return;

    // Decide bid or ask
    bool is_bid = uniform_dist_(rng_) < 0.5;

    // Get current best price
    double base_price = is_bid ? book->get_best_bid() : book->get_best_ask();
    if (base_price <= 0)
        return;

    // Apply price movement
    double new_price = base_price + calculate_price_movement(base_price);

    // Ensure price stays on tick
    double tick_size = 0.25; // TODO: Get from instrument config
    new_price = std::round(new_price / tick_size) * tick_size;

    // Generate quantity
    int32_t quantity = calculate_quantity();

    // Create update message
    MDUpdateAction action = quantity > 0 ? MDUpdateAction::Change : MDUpdateAction::Delete;
    uint8_t level = 1; // Update top level

    auto update = create_price_level_update(
        security_id, action,
        is_bid ? MDEntryType::Bid : MDEntryType::Offer,
        new_price, quantity, level, rpt_seq_++);

    // Send update
    inc_publisher_->publish_batch(update);

    // Update the book
    if (is_bid) {
        book->update_bid(new_price, quantity);
    } else {
        book->update_ask(new_price, quantity);
    }
}

void MarketDataGenerator::generate_trade(uint32_t security_id)
{
    auto book = book_manager_->get_order_book(security_id);
    if (!book)
        return;

    double bid = book->get_best_bid();
    double ask = book->get_best_ask();

    if (bid <= 0 || ask <= 0)
        return;

    // Trade happens at bid or ask
    double trade_price = uniform_dist_(rng_) < 0.5 ? bid : ask;
    int32_t quantity = calculate_quantity();
    char aggressor = trade_price == ask ? 'B' : 'S';

    // Create trade message
    auto update = create_trade_update(security_id, trade_price, quantity, aggressor, rpt_seq_++);

    // Send update
    inc_publisher_->publish_batch(update);

    // Update the book
    book->add_trade(trade_price, quantity, aggressor);

    // Remove or reduce the touched level
    if (aggressor == 'B') {
        auto asks = book->get_asks(1);
        if (!asks.empty()) {
            int64_t remaining = static_cast<int64_t>(asks[0].quantity) - quantity;
            book->update_ask(trade_price, remaining > 0 ? remaining : 0);
        }
    } else {
        auto bids = book->get_bids(1);
        if (!bids.empty()) {
            int64_t remaining = static_cast<int64_t>(bids[0].quantity) - quantity;
            book->update_bid(trade_price, remaining > 0 ? remaining : 0);
        }
    }

    trades_generated_++;

    LOG_DEBUG("Generated trade for " + book->get_symbol() + ": " + std::to_string(quantity) + " @ " + std::to_string(trade_price) + " (" + aggressor + ")");
}

double MarketDataGenerator::calculate_price_movement(double current_price)
{
    // Base movement from normal distribution
    double movement = normal_dist_(rng_) * config_.volatility * current_price;

    // Add trend bias
    movement += config_.trend_bias * config_.volatility * current_price;

    // Adjust for market mode
    switch (config_.mode) {
    case MarketMode::Fast:
        movement *= 2.0;
        break;
    case MarketMode::Volatile:
        movement *= 3.0;
        break;
    case MarketMode::Thin:
        movement *= 1.5;
        break;
    default:
        break;
    }

    return movement;
}

int MarketDataGenerator::calculate_quantity()
{
    // Base quantity between 1 and 10 lots (100 shares per lot)
    std::uniform_int_distribution<> qty_dist(1, 10);
    int lots = qty_dist(rng_);

    // Adjust for market mode
    switch (config_.mode) {
    case MarketMode::Fast:
        lots *= 2;
        break;
    case MarketMode::Thin:
        lots = std::max(1, lots / 2);
        break;
    default:
        break;
    }

    // Occasionally generate zero quantity (cancel)
    if (uniform_dist_(rng_) < 0.1) {
        return 0;
    }

    return lots * 100;
}

bool MarketDataGenerator::should_generate_trade()
{
    double trade_prob = config_.trade_probability;

    // Adjust for market mode
    switch (config_.mode) {
    case MarketMode::Fast:
        trade_prob *= 1.5;
        break;
    case MarketMode::Thin:
        trade_prob *= 0.5;
        break;
    default:
        break;
    }

    return uniform_dist_(rng_) < trade_prob;
}

} // namespace cme_mock