#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cme_mock {

class UDPPublisher {
public:
    UDPPublisher(const std::string& ip_address, uint16_t port);
    ~UDPPublisher();

    // Initialize the socket
    bool initialize();

    // Send data
    bool send(const uint8_t* data, size_t length);
    bool send(const std::vector<uint8_t>& data);

    // Batch sending for efficiency
    void queue_message(const std::vector<uint8_t>& message);
    bool flush_queue();

    // Statistics
    uint64_t get_messages_sent() const { return messages_sent_; }
    uint64_t get_bytes_sent() const { return bytes_sent_; }
    uint64_t get_errors() const { return errors_; }

private:
    std::string ip_address_;
    uint16_t port_;
    int socket_fd_;

    // Message batching
    std::vector<std::vector<uint8_t>> message_queue_;
    size_t max_batch_size_ = 10;

    // Statistics
    uint64_t messages_sent_ = 0;
    uint64_t bytes_sent_ = 0;
    uint64_t errors_ = 0;

    void cleanup();
};

} // namespace cme_mock