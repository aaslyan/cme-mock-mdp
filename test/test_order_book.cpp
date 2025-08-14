#include <iostream>
#include <iomanip>
#include "core/order_book.h"
#include "core/order_book_manager.h"
#include "utils/logger.h"

void print_book_state(const cme_mock::OrderBook& book) {
    std::cout << "\n=== Order Book: " << book.get_symbol() << " ===" << std::endl;
    
    // Print asks (reverse order for display)
    auto asks = book.get_asks(5);
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
        std::cout << std::setw(10) << " " 
                  << std::setw(10) << it->quantity
                  << " @ " << std::fixed << std::setprecision(2) 
                  << std::setw(10) << it->price << " ASK" << std::endl;
    }
    
    std::cout << std::string(40, '-') << std::endl;
    
    // Print spread
    double spread = book.get_spread();
    double mid = book.get_mid_price();
    std::cout << "   Spread: " << std::fixed << std::setprecision(2) << spread 
              << "   Mid: " << mid << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    // Print bids
    auto bids = book.get_bids(5);
    for (const auto& bid : bids) {
        std::cout << "BID " << std::fixed << std::setprecision(2) 
                  << std::setw(10) << bid.price << " @ "
                  << std::setw(10) << bid.quantity << std::endl;
    }
    
    // Print stats
    const auto& stats = book.get_stats();
    std::cout << "\nStats - Last: " << stats.last_price 
              << " Volume: " << stats.total_volume
              << " Trades: " << stats.trade_count << std::endl;
}

int main() {
    std::cout << "Testing Order Book Implementation\n" << std::endl;
    
    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);
    
    // Test 1: Basic Order Book Operations
    std::cout << "=== Test 1: Basic Operations ===" << std::endl;
    
    cme_mock::OrderBook book(1, "ES");
    
    // Add some bids
    book.add_bid(4500.00, 100);
    book.add_bid(4499.75, 200);
    book.add_bid(4499.50, 300);
    book.add_bid(4499.25, 150);
    
    // Add some asks
    book.add_ask(4500.25, 100);
    book.add_ask(4500.50, 250);
    book.add_ask(4500.75, 300);
    book.add_ask(4501.00, 400);
    
    print_book_state(book);
    
    // Test 2: Order Book Manager
    std::cout << "\n=== Test 2: Order Book Manager ===" << std::endl;
    
    cme_mock::OrderBookManager manager;
    
    // Create multiple order books
    manager.create_order_book(1, "ES", 0.25, 4500.00, 0.25);
    manager.create_order_book(2, "NQ", 0.25, 15500.00, 0.25);
    manager.create_order_book(3, "CL", 0.01, 75.50, 0.01);
    
    std::cout << "Created " << manager.book_count() << " order books" << std::endl;
    
    // Get and display ES book
    auto es_book = manager.get_order_book(1);
    if (es_book) {
        print_book_state(*es_book);
    }
    
    // Test 3: Trades and Updates
    std::cout << "\n=== Test 3: Trades and Updates ===" << std::endl;
    
    // Simulate some trades
    es_book->add_trade(4500.25, 50, 'B');  // Buy aggressor
    es_book->update_ask(4500.25, 50);      // Reduce ask quantity
    
    es_book->add_trade(4500.00, 100, 'S'); // Sell aggressor
    es_book->update_bid(4500.00, 0);       // Remove bid level
    
    print_book_state(*es_book);
    
    // Test 4: Market Conditions
    std::cout << "\n=== Test 4: Market Conditions ===" << std::endl;
    
    // Test crossed market
    book.clear();
    book.add_bid(100.00, 100);
    book.add_ask(99.00, 100);
    std::cout << "Crossed market test: " << (book.is_crossed() ? "CROSSED" : "NORMAL") << std::endl;
    
    // Test empty book
    book.clear();
    std::cout << "Empty book - Bid: " << book.get_best_bid() 
              << " Ask: " << book.get_best_ask() 
              << " Mid: " << book.get_mid_price() << std::endl;
    
    // Test 5: Performance
    std::cout << "\n=== Test 5: Performance ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Add many levels
    for (int i = 0; i < 1000; ++i) {
        book.add_bid(4500.00 - i * 0.25, 100);
        book.add_ask(4501.00 + i * 0.25, 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Added 2000 levels in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Book depth - Bids: " << book.bid_depth() 
              << " Asks: " << book.ask_depth() << std::endl;
    
    return 0;
}