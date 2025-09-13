#pragma once

#include "../../core/include/market_data_generator.h"
#include "../../core/include/market_events.h"
#include "../../protocols/common/include/tcp_transport.h"
#include "lseg_sbe/Establish.h"
#include "lseg_sbe/MessageHeader.h"
#include "lseg_sbe/Negotiate.h"
#include "reuters_encoder.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace reuters_protocol {

// Session state for each connected client
enum class SessionState {
    DISCONNECTED,
    NEGOTIATING,
    NEGOTIATED,
    ESTABLISHING,
    ESTABLISHED,
    TERMINATED
};

struct ClientSession {
    std::string session_id;
    SessionState state;
    FlowType flow_type;
    uint32_t keepalive_interval;
    std::chrono::steady_clock::time_point last_activity;
    int socket_fd;
    std::string username;

    // Market data subscriptions
    std::vector<std::string> subscribed_instruments;
    bool snapshot_requested;
    bool incremental_requested;

    ClientSession()
        : state(SessionState::DISCONNECTED)
        , flow_type(FlowType::UNSEQUENCED)
        , keepalive_interval(30000)
        , socket_fd(-1)
        , snapshot_requested(false)
        , incremental_requested(false)
    {
    }
};

class ReutersProtocolAdapter : public market_core::IMarketEventListener {
public:
    explicit ReutersProtocolAdapter(uint16_t port = 11501);
    ~ReutersProtocolAdapter();

    // IMarketEventListener implementation
    void on_market_event(const std::shared_ptr<market_core::MarketEvent>& event) override;

    // Single-threaded server operations
    bool initialize();
    void run_once(); // Process one iteration of server loop
    void shutdown();

    // Statistics
    struct Statistics {
        size_t sessions_created;
        size_t messages_sent;
        size_t messages_received;
        size_t market_events_processed;
        std::chrono::steady_clock::time_point start_time;
    };

    const Statistics& get_statistics() const { return stats_; }

private:
    uint16_t port_;
    int server_socket_;
    bool running_;
    Statistics stats_;

    std::unordered_map<int, std::unique_ptr<ClientSession>> sessions_;

    // Core server operations
    void accept_new_connections();
    void process_client_messages();
    void send_heartbeats();
    void cleanup_inactive_sessions();

    // Message handling
    void handle_client_message(ClientSession& session, const std::vector<uint8_t>& data);
    void handle_negotiate(ClientSession& session, const std::vector<uint8_t>& payload);
    void handle_establish(ClientSession& session, const std::vector<uint8_t>& payload);
    void handle_market_data_request(ClientSession& session, const std::vector<uint8_t>& payload);
    void handle_security_definition_request(ClientSession& session, const std::vector<uint8_t>& payload);

    // Market data distribution
    void send_to_session(ClientSession& session, const std::vector<uint8_t>& message);
    void broadcast_to_subscribed_sessions(const std::vector<uint8_t>& message,
        const std::string& instrument_id = "");

    // Session management
    std::string generate_session_id();
    bool validate_credentials(const std::string& username, const std::string& password);
    void terminate_session(ClientSession& session, TerminationCode code, const std::string& reason);

    // Message parsing helpers
    ReutersMessage parse_message(const std::vector<uint8_t>& data);
    bool is_complete_message(const std::vector<uint8_t>& buffer);

    // Network utilities
    std::vector<uint8_t> receive_from_socket(int socket_fd);
    bool send_to_socket(int socket_fd, const std::vector<uint8_t>& data);

    static constexpr size_t MAX_BUFFER_SIZE = 8192;
    static constexpr uint32_t DEFAULT_KEEPALIVE_MS = 30000;
    static constexpr uint32_t SESSION_TIMEOUT_MS = 60000;
};

} // namespace reuters_protocol