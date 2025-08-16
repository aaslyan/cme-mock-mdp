#include "network/udp_publisher.h"
#include "utils/logger.h"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace cme_mock {

UDPPublisher::UDPPublisher(const std::string& ip_address, uint16_t port)
    : ip_address_(ip_address)
    , port_(port)
    , socket_fd_(-1)
{
}

UDPPublisher::~UDPPublisher()
{
    cleanup();
}

bool UDPPublisher::initialize()
{
    // Create UDP socket
    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd_ < 0) {
        LOG_ERROR("Failed to create UDP socket: " + std::string(strerror(errno)));
        return false;
    }

    // Set socket options
    int enable = 1;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        LOG_WARNING("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    // Set send buffer size for better performance
    int send_buffer_size = 1024 * 1024; // 1MB
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_SNDBUF, &send_buffer_size, sizeof(send_buffer_size)) < 0) {
        LOG_WARNING("Failed to set send buffer size: " + std::string(strerror(errno)));
    }

    LOG_INFO("UDP publisher initialized for " + ip_address_ + ":" + std::to_string(port_));
    return true;
}

bool UDPPublisher::send(const uint8_t* data, size_t length)
{
    if (socket_fd_ < 0) {
        LOG_ERROR("Socket not initialized");
        return false;
    }

    struct sockaddr_in dest_addr;
    std::memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, ip_address_.c_str(), &dest_addr.sin_addr) <= 0) {
        LOG_ERROR("Invalid IP address: " + ip_address_);
        errors_++;
        return false;
    }

    ssize_t sent = sendto(socket_fd_, data, length, 0,
        (struct sockaddr*)&dest_addr, sizeof(dest_addr));

    if (sent < 0) {
        LOG_ERROR("Failed to send UDP packet: " + std::string(strerror(errno)));
        errors_++;
        return false;
    }

    if (static_cast<size_t>(sent) != length) {
        LOG_WARNING("Partial send: " + std::to_string(sent) + " of " + std::to_string(length) + " bytes");
    }

    messages_sent_++;
    bytes_sent_ += sent;

    return true;
}

bool UDPPublisher::send(const std::vector<uint8_t>& data)
{
    return send(data.data(), data.size());
}

void UDPPublisher::queue_message(const std::vector<uint8_t>& message)
{
    message_queue_.push_back(message);

    // Auto-flush if queue reaches max batch size
    if (message_queue_.size() >= max_batch_size_) {
        flush_queue();
    }
}

bool UDPPublisher::flush_queue()
{
    bool all_sent = true;

    for (const auto& message : message_queue_) {
        if (!send(message)) {
            all_sent = false;
        }
    }

    message_queue_.clear();
    return all_sent;
}

void UDPPublisher::cleanup()
{
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

} // namespace cme_mock