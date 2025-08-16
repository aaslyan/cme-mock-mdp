#include "network/udp_receiver.h"
#include "utils/logger.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace cme_mock {

UDPReceiver::UDPReceiver(const std::string& ip_address, uint16_t port)
    : ip_address_(ip_address)
    , port_(port)
    , socket_fd_(-1)
{
}

UDPReceiver::~UDPReceiver()
{
    cleanup();
}

bool UDPReceiver::initialize()
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

    // Set SO_REUSEPORT for multicast (allows multiple receivers)
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) < 0) {
        LOG_WARNING("Failed to set SO_REUSEPORT: " + std::string(strerror(errno)));
    }

    // Bind to port
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR("Failed to bind to port " + std::to_string(port_) + ": " + std::string(strerror(errno)));
        cleanup();
        return false;
    }

    // Set receive buffer size
    int recv_buffer_size = 1024 * 1024; // 1MB
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_RCVBUF, &recv_buffer_size, sizeof(recv_buffer_size)) < 0) {
        LOG_WARNING("Failed to set receive buffer size: " + std::string(strerror(errno)));
    }

    // Check if this is a multicast address and join the group
    struct in_addr addr_check;
    if (inet_pton(AF_INET, ip_address_.c_str(), &addr_check) > 0) {
        uint32_t ip_host = ntohl(addr_check.s_addr);
        if (ip_host >= 0xE0000000 && ip_host <= 0xEFFFFFFF) {
            // This is a multicast address, join the multicast group
            LOG_INFO("Joining multicast group: " + ip_address_);
            
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = addr_check.s_addr;
            mreq.imr_interface.s_addr = INADDR_ANY;
            
            if (setsockopt(socket_fd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
                LOG_ERROR("Failed to join multicast group: " + std::string(strerror(errno)));
                cleanup();
                return false;
            }
        }
    }

    LOG_INFO("UDP receiver initialized for " + ip_address_ + ":" + std::to_string(port_));
    return true;
}

bool UDPReceiver::receive_messages(int timeout_ms)
{
    if (socket_fd_ < 0) {
        LOG_ERROR("Socket not initialized");
        return false;
    }

    // Set receive timeout
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    if (setsockopt(socket_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        LOG_WARNING("Failed to set receive timeout: " + std::string(strerror(errno)));
    }

    char buffer[65536];
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);

    while (true) {
        ssize_t received = recvfrom(socket_fd_, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&sender_addr, &sender_len);

        if (received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout
                return true;
            } else {
                LOG_ERROR("Failed to receive UDP packet: " + std::string(strerror(errno)));
                errors_++;
                return false;
            }
        }

        if (received == 0) {
            continue;
        }

        messages_received_++;
        bytes_received_ += received;

        // Call callback if set
        if (message_callback_) {
            std::vector<uint8_t> data(buffer, buffer + received);
            message_callback_(data);
        }
    }

    return true;
}

bool UDPReceiver::receive_once(std::vector<uint8_t>& data, int timeout_ms)
{
    if (socket_fd_ < 0) {
        LOG_ERROR("Socket not initialized");
        return false;
    }

    // Set receive timeout
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    if (setsockopt(socket_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        LOG_WARNING("Failed to set receive timeout: " + std::string(strerror(errno)));
    }

    char buffer[65536];
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);

    ssize_t received = recvfrom(socket_fd_, buffer, sizeof(buffer), 0,
        (struct sockaddr*)&sender_addr, &sender_len);

    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Timeout
            return false;
        } else {
            LOG_ERROR("Failed to receive UDP packet: " + std::string(strerror(errno)));
            errors_++;
            return false;
        }
    }

    if (received == 0) {
        return false;
    }

    messages_received_++;
    bytes_received_ += received;

    data.assign(buffer, buffer + received);
    return true;
}

void UDPReceiver::cleanup()
{
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

} // namespace cme_mock