#include "../include/reuters_protocol_adapter.h"
#include "../../protocols/common/include/tcp_transport.h"
#include "../include/reuters_encoder.h"
#include <algorithm>
#include <iostream>
#include <poll.h>
#include <random>
#include <sstream>
#include <sys/socket.h>

namespace reuters_protocol {

ReutersProtocolAdapter::ReutersProtocolAdapter(uint16_t port)
    : port_(port)
    , server_socket_(-1)
    , running_(false)
    , stats_ {}
    , use_multicast_(false)
{
    stats_.start_time = std::chrono::steady_clock::now();
}

ReutersProtocolAdapter::ReutersProtocolAdapter(uint16_t port, const ReutersMulticastConfig& multicast_config)
    : port_(port)
    , server_socket_(-1)
    , running_(false)
    , stats_ {}
    , use_multicast_(true)
    , multicast_config_(multicast_config)
{
    stats_.start_time = std::chrono::steady_clock::now();
}

ReutersProtocolAdapter::~ReutersProtocolAdapter()
{
    shutdown();
}

bool ReutersProtocolAdapter::initialize()
{
    try {
        server_socket_ = protocol_common::TCPTransport::create_server_socket(port_);
        protocol_common::TCPTransport::set_non_blocking(server_socket_);

        running_ = true;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ReutersProtocolAdapter::initialize_with_multicast()
{
    // Initialize TCP for session management
    if (!initialize()) {
        return false;
    }

    // Initialize multicast publisher if configured
    if (use_multicast_) {
        multicast_publisher_ = std::make_unique<ReutersMulticastPublisher>(multicast_config_);
        if (!multicast_publisher_->initialize()) {
            std::cerr << "Failed to initialize multicast publisher" << std::endl;
            return false;
        }
        std::cout << "Reuters multicast publisher initialized successfully" << std::endl;
    }

    return true;
}

void ReutersProtocolAdapter::run_once()
{
    if (!running_)
        return;

    // Single-threaded event loop iteration
    accept_new_connections();
    process_client_messages();
    send_heartbeats();
    cleanup_inactive_sessions();

    // Send multicast heartbeats if needed
    if (use_multicast_ && multicast_publisher_) {
        static auto last_heartbeat = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat).count() >= 30) {
            multicast_publisher_->send_heartbeat();
            last_heartbeat = now;
        }
    }
}

void ReutersProtocolAdapter::shutdown()
{
    running_ = false;

    // Shutdown multicast publisher
    if (multicast_publisher_) {
        multicast_publisher_->shutdown();
        multicast_publisher_.reset();
    }

    // Terminate all active sessions
    for (auto& [socket_fd, session] : sessions_) {
        if (session->state == SessionState::ESTABLISHED) {
            auto terminate_msg = ReutersEncoder::encode_terminate(
                session->session_id,
                TerminationCode::FINISHED,
                "Server shutdown");
            send_to_session(*session, terminate_msg);
        }
        protocol_common::TCPTransport::close_socket(socket_fd);
    }
    sessions_.clear();

    if (server_socket_ >= 0) {
        protocol_common::TCPTransport::close_socket(server_socket_);
        server_socket_ = -1;
    }
}

void ReutersProtocolAdapter::on_market_event(
    const std::shared_ptr<market_core::MarketEvent>& event)
{
    if (!running_)
        return;

    stats_.market_events_processed++;

    // If multicast is enabled, publish to multicast channels
    if (use_multicast_ && multicast_publisher_) {
        switch (event->type) {
        case market_core::MarketEvent::QUOTE_UPDATE: {
            auto quote = std::static_pointer_cast<market_core::QuoteEvent>(event);
            multicast_publisher_->publish_incremental(*quote);
            break;
        }
        case market_core::MarketEvent::TRADE: {
            auto trade = std::static_pointer_cast<market_core::TradeEvent>(event);
            multicast_publisher_->publish_incremental(*trade);
            break;
        }
        case market_core::MarketEvent::SNAPSHOT: {
            auto snapshot = std::static_pointer_cast<market_core::SnapshotEvent>(event);
            multicast_publisher_->publish_snapshot(*snapshot);
            break;
        }
        case market_core::MarketEvent::STATISTICS: {
            auto stats = std::static_pointer_cast<market_core::StatisticsEvent>(event);
            multicast_publisher_->publish_statistics(*stats);
            break;
        }
        default:
            break;
        }
    }

    // Also send to TCP sessions if they exist
    switch (event->type) {
    case market_core::MarketEvent::QUOTE_UPDATE: {
        auto quote = std::static_pointer_cast<market_core::QuoteEvent>(event);
        auto msg = ReutersEncoder::encode_market_data_incremental(*quote);
        broadcast_to_subscribed_sessions(msg, std::to_string(quote->instrument_id));
        break;
    }

    case market_core::MarketEvent::TRADE: {
        auto trade = std::static_pointer_cast<market_core::TradeEvent>(event);
        auto msg = ReutersEncoder::encode_market_data_incremental(*trade);
        broadcast_to_subscribed_sessions(msg, std::to_string(trade->instrument_id));
        break;
    }

    case market_core::MarketEvent::SNAPSHOT: {
        auto snapshot = std::static_pointer_cast<market_core::SnapshotEvent>(event);
        auto msg = ReutersEncoder::encode_market_data_snapshot(*snapshot);
        broadcast_to_subscribed_sessions(msg, std::to_string(snapshot->instrument_id));
        break;
    }

    case market_core::MarketEvent::STATISTICS:
    case market_core::MarketEvent::STATUS_CHANGE:
    case market_core::MarketEvent::BOOK_CLEAR:
    case market_core::MarketEvent::IMBALANCE:
        // Could implement these later if needed
        break;
    }
}

void ReutersProtocolAdapter::accept_new_connections()
{
    int client_fd = protocol_common::TCPTransport::accept_connection(server_socket_);
    if (client_fd < 0)
        return; // No pending connections

    // Set client socket options
    protocol_common::TCPTransport::set_non_blocking(client_fd);
    protocol_common::TCPTransport::set_socket_options(client_fd);

    // Create new session
    auto session = std::make_unique<ClientSession>();
    session->socket_fd = client_fd;
    session->state = SessionState::DISCONNECTED;
    session->last_activity = std::chrono::steady_clock::now();

    sessions_[client_fd] = std::move(session);
    stats_.sessions_created++;
}

void ReutersProtocolAdapter::process_client_messages()
{
    // Use poll to check all client sockets for data
    std::vector<pollfd> poll_fds;
    for (const auto& [socket_fd, session] : sessions_) {
        poll_fds.push_back({ socket_fd, POLLIN, 0 });
    }

    if (poll_fds.empty())
        return;

    int ready = poll(poll_fds.data(), poll_fds.size(), 0); // Non-blocking
    if (ready <= 0)
        return;

    for (const auto& pfd : poll_fds) {
        if (!(pfd.revents & POLLIN))
            continue;

        auto session_it = sessions_.find(pfd.fd);
        if (session_it == sessions_.end())
            continue;

        auto& session = *session_it->second;

        try {
            auto data = protocol_common::TCPTransport::receive_data(pfd.fd);
            if (!data.empty()) {
                session.last_activity = std::chrono::steady_clock::now();
                handle_client_message(session, data);
                stats_.messages_received++;
            }
        } catch (const std::exception& e) {
            // Connection closed or error
            terminate_session(session, TerminationCode::UNSPECIFIED_ERROR, "Connection error");
        }
    }
}

void ReutersProtocolAdapter::handle_client_message(
    ClientSession& session,
    const std::vector<uint8_t>& data)
{
    if (data.size() < SOFHeader::size())
        return;

    // Parse SOFH header
    SOFHeader sofh;
    sofh.unpack(data.data());

    if (data.size() < sofh.message_length)
        return; // Incomplete message

    // Extract SBE message
    const uint8_t* sbe_data = data.data() + SOFHeader::size();
    size_t sbe_size = sofh.message_length - SOFHeader::size();

    if (sbe_size < lseg_sbe::MessageHeader::encodedLength())
        return;

    // Parse SBE message header to get template ID
    lseg_sbe::MessageHeader msg_header;
    msg_header.wrap(const_cast<char*>(reinterpret_cast<const char*>(sbe_data)),
        0, 0, sbe_size);

    uint16_t template_id = msg_header.templateId();

    // Route message based on template ID
    switch (template_id) {
    case MessageTypes::NEGOTIATE:
        handle_negotiate(session, { sbe_data, sbe_data + sbe_size });
        break;

    case MessageTypes::ESTABLISH:
        handle_establish(session, { sbe_data, sbe_data + sbe_size });
        break;

    case MessageTypes::MARKET_DATA_REQUEST:
        handle_market_data_request(session, { sbe_data, sbe_data + sbe_size });
        break;

    case MessageTypes::SECURITY_DEFINITION_REQUEST:
        handle_security_definition_request(session, { sbe_data, sbe_data + sbe_size });
        break;

    case MessageTypes::HEARTBEAT:
        // Client heartbeat - just update last activity (already done above)
        break;

    default:
        // Unknown message type
        break;
    }
}

void ReutersProtocolAdapter::handle_negotiate(
    ClientSession& session,
    const std::vector<uint8_t>& payload)
{
    if (session.state != SessionState::DISCONNECTED) {
        terminate_session(session, TerminationCode::UNSPECIFIED_ERROR, "Invalid state for negotiate");
        return;
    }

    // Parse negotiate message to extract credentials
    lseg_sbe::Negotiate negotiate;
    negotiate.wrapForDecode(const_cast<char*>(reinterpret_cast<const char*>(payload.data())),
        0, negotiate.sbeBlockLength(), negotiate.sbeSchemaVersion(), payload.size());

    // Extract session ID and credentials
    std::string session_id(negotiate.sessionID(), negotiate.sessionIDLength());
    // Note: Credentials extraction would need proper SBE composite handling

    session.session_id = session_id;
    session.state = SessionState::NEGOTIATING;

    // For simplicity, accept all negotiations
    // In production, would validate credentials here
    bool authenticated = true;

    if (authenticated) {
        auto response = ReutersEncoder::encode_negotiate_response(
            session_id, FlowType::UNSEQUENCED);
        send_to_session(session, response);
        session.state = SessionState::NEGOTIATED;
    } else {
        auto reject = ReutersEncoder::encode_negotiate_reject(
            session_id, SessionRejectCode::CREDENTIALS, "Invalid credentials");
        send_to_session(session, reject);
        terminate_session(session, TerminationCode::UNSPECIFIED_ERROR, "Authentication failed");
    }
}

void ReutersProtocolAdapter::handle_establish(
    ClientSession& session,
    const std::vector<uint8_t>& payload)
{
    if (session.state != SessionState::NEGOTIATED) {
        terminate_session(session, TerminationCode::UNSPECIFIED_ERROR, "Invalid state for establish");
        return;
    }

    // Parse establish message
    lseg_sbe::Establish establish;
    establish.wrapForDecode(const_cast<char*>(reinterpret_cast<const char*>(payload.data())),
        0, establish.sbeBlockLength(), establish.sbeSchemaVersion(), payload.size());

    session.keepalive_interval = establish.keepAliveInterval();
    session.state = SessionState::ESTABLISHING;

    // Send establishment acknowledgment
    auto ack = ReutersEncoder::encode_establish_ack(
        session.session_id, session.keepalive_interval);
    send_to_session(session, ack);

    session.state = SessionState::ESTABLISHED;
}

void ReutersProtocolAdapter::handle_market_data_request(
    ClientSession& session,
    const std::vector<uint8_t>& payload)
{
    if (session.state != SessionState::ESTABLISHED)
        return;

    // Parse market data request
    // For simplicity, subscribe to all instruments
    session.incremental_requested = true;
    session.snapshot_requested = true;

    // Could parse specific instrument subscriptions from payload
    session.subscribed_instruments.clear();
    session.subscribed_instruments.push_back("all"); // Subscribe to all

    (void)payload; // Suppress unused warning
}

void ReutersProtocolAdapter::handle_security_definition_request(
    ClientSession& session,
    const std::vector<uint8_t>& payload)
{
    if (session.state != SessionState::ESTABLISHED)
        return;

    // Send security definitions for all our instruments
    // This would typically come from our core instrument database
    // For now, send a sample FX instrument definition
    market_core::Instrument sample_fx(1001, "EURUSD", market_core::InstrumentType::FX_SPOT);
    sample_fx.tick_size = 0.00001;

    (void)payload; // Suppress unused warning

    auto sec_def = ReutersEncoder::encode_security_definition(sample_fx);
    send_to_session(session, sec_def);
}

void ReutersProtocolAdapter::send_heartbeats()
{
    auto now = std::chrono::steady_clock::now();

    for (auto& [socket_fd, session] : sessions_) {
        if (session->state != SessionState::ESTABLISHED)
            continue;

        auto time_since_activity = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - session->last_activity)
                                       .count();

        if (time_since_activity > session->keepalive_interval / 2) {
            auto heartbeat = ReutersEncoder::encode_heartbeat();
            send_to_session(*session, heartbeat);
        }
    }
}

void ReutersProtocolAdapter::cleanup_inactive_sessions()
{
    auto now = std::chrono::steady_clock::now();

    for (auto it = sessions_.begin(); it != sessions_.end();) {
        auto& session = *it->second;

        auto inactive_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - session.last_activity)
                                 .count();

        if (inactive_time > SESSION_TIMEOUT_MS) {
            terminate_session(session, TerminationCode::UNSPECIFIED_ERROR, "Session timeout");
            protocol_common::TCPTransport::close_socket(it->first);
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

void ReutersProtocolAdapter::send_to_session(
    ClientSession& session,
    const std::vector<uint8_t>& message)
{
    if (protocol_common::TCPTransport::send_data(session.socket_fd, message)) {
        stats_.messages_sent++;
    }
}

void ReutersProtocolAdapter::broadcast_to_subscribed_sessions(
    const std::vector<uint8_t>& message,
    const std::string& instrument_id)
{
    for (auto& [socket_fd, session] : sessions_) {
        if (session->state != SessionState::ESTABLISHED)
            continue;
        if (!session->incremental_requested && !session->snapshot_requested)
            continue;

        // Check if session is subscribed to this instrument
        if (!session->subscribed_instruments.empty() && session->subscribed_instruments[0] != "all") {

            auto it = std::find(session->subscribed_instruments.begin(),
                session->subscribed_instruments.end(),
                instrument_id);
            if (it == session->subscribed_instruments.end())
                continue;
        }

        send_to_session(*session, message);
    }
}

std::string ReutersProtocolAdapter::generate_session_id()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << "SID";
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

bool ReutersProtocolAdapter::validate_credentials(
    const std::string& username,
    const std::string& password)
{
    // Simple validation - in production would check against database
    return username == "CISA_MD_01" && password == "mit123";
}

void ReutersProtocolAdapter::terminate_session(
    ClientSession& session,
    TerminationCode code,
    const std::string& reason)
{
    if (session.state != SessionState::TERMINATED) {
        auto terminate_msg = ReutersEncoder::encode_terminate(
            session.session_id, code, reason);
        send_to_session(session, terminate_msg);
        session.state = SessionState::TERMINATED;
    }
}

} // namespace reuters_protocol