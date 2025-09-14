#pragma once

#include "../../test/utp_client/UTPMessages.h"
#include <memory>
#include <string>
#include <vector>

namespace utp_simple {

// Simple UTP message sender that works without the complex protocol adapter
class UTPMessageSender {
private:
    int m_socket = -1;
    std::string m_multicast_ip;
    int m_port;
    uint64_t m_sequence = 1;

public:
    UTPMessageSender(const std::string& ip, int port);
    ~UTPMessageSender();

    bool initialize();
    void cleanup();

    // Send UTP messages
    bool send_heartbeat();
    bool send_security_definition(uint32_t security_id, const std::string& symbol);
    bool send_snapshot(uint32_t security_id, double bid_price, double ask_price,
        int64_t bid_size, int64_t ask_size);
    bool send_incremental_update(uint32_t security_id, double bid_price, double ask_price,
        int64_t bid_size, int64_t ask_size);

private:
    bool send_multicast_message(const void* data, size_t size);
    uint64_t get_current_timestamp_ns();
};

} // namespace utp_simple