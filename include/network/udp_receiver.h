#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace cme_mock {

class UDPReceiver {
public:
    using MessageCallback = std::function<void(const std::vector<uint8_t>&)>;

    UDPReceiver(const std::string& ip_address, uint16_t port);
    ~UDPReceiver();

    // Initialize the socket
    bool initialize();

    // Set callback for received messages
    void set_message_callback(MessageCallback callback)
    {
        message_callback_ = callback;
    }

    // Receive messages (blocking)
    bool receive_messages(int timeout_ms = 1000);

    // Receive a single message
    bool receive_once(std::vector<uint8_t>& data, int timeout_ms = 1000);

    // Statistics
    uint64_t get_messages_received() const { return messages_received_; }
    uint64_t get_bytes_received() const { return bytes_received_; }
    uint64_t get_errors() const { return errors_; }

private:
    std::string ip_address_;
    uint16_t port_;
    int socket_fd_;

    MessageCallback message_callback_;

    // Statistics
    uint64_t messages_received_ = 0;
    uint64_t bytes_received_ = 0;
    uint64_t errors_ = 0;

    void cleanup();
};

} // namespace cme_mock