#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "order_book.h"

namespace cme_mock {

class OrderBookManager {
public:
    OrderBookManager() = default;
    ~OrderBookManager() = default;
    
    // Order book management
    bool create_order_book(uint32_t security_id, 
                          const std::string& symbol,
                          double tick_size,
                          double initial_price,
                          double initial_spread);
    
    std::shared_ptr<OrderBook> get_order_book(uint32_t security_id);
    std::vector<uint32_t> get_all_security_ids() const;
    
    // Bulk operations
    void clear_all_books();
    size_t book_count() const;
    
private:
    std::unordered_map<uint32_t, std::shared_ptr<OrderBook>> order_books_;
};

} // namespace cme_mock