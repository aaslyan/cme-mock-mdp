#include "core/order_book_manager.h"
#include "utils/logger.h"
#include <sstream>

namespace cme_mock {

bool OrderBookManager::create_order_book(uint32_t security_id, 
                                       const std::string& symbol,
                                       double tick_size,
                                       double initial_price,
                                       double initial_spread) {
    // Check if order book already exists
    if (order_books_.find(security_id) != order_books_.end()) {
        LOG_WARNING("Order book for security " + std::to_string(security_id) + " already exists");
        return false;
    }
    
    // Create new order book
    auto order_book = std::make_shared<OrderBook>(security_id, symbol);
    
    // Initialize with some default levels if initial price is provided
    if (initial_price > 0) {
        // Add some bid levels
        for (int i = 1; i <= 5; ++i) {
            double price = initial_price - (i * tick_size);
            uint64_t quantity = 100 * i;  // Increasing quantity at deeper levels
            order_book->add_bid(price, quantity);
        }
        
        // Add some ask levels
        for (int i = 1; i <= 5; ++i) {
            double price = initial_price + (initial_spread) + ((i - 1) * tick_size);
            uint64_t quantity = 100 * i;
            order_book->add_ask(price, quantity);
        }
        
        LOG_INFO("Initialized order book for " + symbol + " with price " + 
                 std::to_string(initial_price) + " and spread " + std::to_string(initial_spread));
    }
    
    order_books_[security_id] = order_book;
    return true;
}

std::shared_ptr<OrderBook> OrderBookManager::get_order_book(uint32_t security_id) {
    auto it = order_books_.find(security_id);
    if (it != order_books_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<uint32_t> OrderBookManager::get_all_security_ids() const {
    std::vector<uint32_t> ids;
    ids.reserve(order_books_.size());
    
    for (const auto& [id, book] : order_books_) {
        ids.push_back(id);
    }
    
    return ids;
}

void OrderBookManager::clear_all_books() {
    for (auto& [id, book] : order_books_) {
        book->clear();
    }
    LOG_INFO("Cleared all order books");
}

size_t OrderBookManager::book_count() const {
    return order_books_.size();
}

} // namespace cme_mock