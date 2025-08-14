#include <iostream>
#include <thread>
#include <chrono>
#include "network/udp_publisher.h"
#include "utils/logger.h"

int main() {
    std::cout << "Testing UDP Publisher..." << std::endl;
    
    // Initialize logger
    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::DEBUG);
    
    // Create UDP publisher
    cme_mock::UDPPublisher publisher("127.0.0.1", 14310);
    
    if (!publisher.initialize()) {
        std::cerr << "Failed to initialize UDP publisher" << std::endl;
        return 1;
    }
    
    // Send some test messages
    std::string test_messages[] = {
        "TEST_MESSAGE_001",
        "TEST_MESSAGE_002",
        "TEST_MESSAGE_003",
        "MARKET_DATA_UPDATE",
        "SNAPSHOT_REFRESH"
    };
    
    for (const auto& msg : test_messages) {
        std::vector<uint8_t> data(msg.begin(), msg.end());
        
        if (publisher.send(data)) {
            std::cout << "Sent: " << msg << " (" << data.size() << " bytes)" << std::endl;
        } else {
            std::cerr << "Failed to send: " << msg << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Test batch sending
    std::cout << "\nTesting batch mode..." << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::string batch_msg = "BATCH_MSG_" + std::to_string(i);
        std::vector<uint8_t> data(batch_msg.begin(), batch_msg.end());
        publisher.queue_message(data);
    }
    
    // Flush remaining messages
    publisher.flush_queue();
    
    // Print statistics
    std::cout << "\nStatistics:" << std::endl;
    std::cout << "Messages sent: " << publisher.get_messages_sent() << std::endl;
    std::cout << "Bytes sent: " << publisher.get_bytes_sent() << std::endl;
    std::cout << "Errors: " << publisher.get_errors() << std::endl;
    
    return 0;
}