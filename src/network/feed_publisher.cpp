#include "network/feed_publisher.h"
#include "messages/message_factory.h"
#include "messages/sbe_encoder.h"
#include "utils/logger.h"
#include <sstream>

namespace cme_mock {

// Base FeedPublisher implementation
FeedPublisher::FeedPublisher(const std::string& ip, uint16_t port,
    std::shared_ptr<OrderBookManager> book_manager)
    : book_manager_(book_manager)
{
    udp_publisher_ = std::make_shared<UDPPublisher>(ip, port);
}

bool FeedPublisher::initialize()
{
    if (!udp_publisher_->initialize()) {
        LOG_ERROR("Failed to initialize UDP publisher");
        return false;
    }
    return true;
}

// SnapshotFeedPublisher implementation
void SnapshotFeedPublisher::publish_all_snapshots()
{
    auto security_ids = book_manager_->get_all_security_ids();

    LOG_INFO("Publishing snapshots for " + std::to_string(security_ids.size()) + " instruments");

    for (auto security_id : security_ids) {
        publish_snapshot(security_id);
    }
}

void SnapshotFeedPublisher::publish_snapshot(uint32_t security_id)
{
    auto book = book_manager_->get_order_book(security_id);
    if (!book) {
        LOG_WARNING("Order book not found for security " + std::to_string(security_id));
        return;
    }

    // Create snapshot message from order book
    auto snapshot = create_snapshot_from_book(*book, snapshot_sequence_ - 1, snapshot_sequence_);

    // Set packet header info
    snapshot.header.sequence_number = snapshot_sequence_++;
    snapshot.header.sending_time = get_timestamp_ns();
    snapshot.header.msg_count = 1;

    // Encode and send
    auto encoded = encode_snapshot(snapshot);

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent snapshot for " << book->get_symbol()
           << " (ID: " << security_id << ")"
           << " - " << snapshot.entries.size() << " entries"
           << ", " << encoded.size() << " bytes";
        LOG_DEBUG(ss.str());
    } else {
        LOG_ERROR("Failed to send snapshot for security " + std::to_string(security_id));
    }
}

std::vector<uint8_t> SnapshotFeedPublisher::encode_snapshot(const SnapshotFullRefresh& snapshot)
{
    // First encode the packet header
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        snapshot.header.sequence_number,
        snapshot.header.sending_time);

    // Then encode the message
    auto message = MDPMessageEncoder::encode_snapshot_full_refresh(snapshot);

    // Combine them
    std::vector<uint8_t> result;
    result.reserve(packet_header.size() + message.size());
    result.insert(result.end(), packet_header.begin(), packet_header.end());
    result.insert(result.end(), message.begin(), message.end());

    return result;
}

// IncrementalFeedPublisher implementation
void IncrementalFeedPublisher::publish_book_update(uint32_t security_id, const MDPriceLevel& level)
{
    IncrementalRefresh update;
    update.header.sequence_number = sequence_number_++;
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 1;
    update.transact_time = get_timestamp_ns();

    update.price_levels.push_back(level);

    auto encoded = encode_incremental(update);

    if (udp_publisher_->send(encoded)) {
        LOG_DEBUG("Sent book update for security " + std::to_string(security_id));
    }
}

void IncrementalFeedPublisher::publish_trade(uint32_t security_id, const MDTrade& trade)
{
    IncrementalRefresh update;
    update.header.sequence_number = sequence_number_++;
    update.header.sending_time = get_timestamp_ns();
    update.header.msg_count = 1;
    update.transact_time = get_timestamp_ns();

    update.trades.push_back(trade);

    auto encoded = encode_incremental(update);

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent trade for security " << security_id
           << " - " << trade.quantity << " @ " << (trade.price / 100.0);
        LOG_DEBUG(ss.str());
    }
}

void IncrementalFeedPublisher::publish_batch(const IncrementalRefresh& update)
{
    // Copy and update header
    IncrementalRefresh batch = update;
    batch.header.sequence_number = sequence_number_++;
    batch.header.sending_time = get_timestamp_ns();
    batch.header.msg_count = update.price_levels.size() + update.trades.size();

    auto encoded = encode_incremental(batch);

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent batch update - "
           << update.price_levels.size() << " price levels, "
           << update.trades.size() << " trades";
        LOG_DEBUG(ss.str());
    }
}

std::vector<uint8_t> IncrementalFeedPublisher::encode_incremental(const IncrementalRefresh& update)
{
    // First encode the packet header
    auto packet_header = MDPMessageEncoder::encode_packet_header(
        update.header.sequence_number,
        update.header.sending_time);

    // Then encode the message
    auto message = MDPMessageEncoder::encode_incremental_refresh(update);

    // Combine them
    std::vector<uint8_t> result;
    result.reserve(packet_header.size() + message.size());
    result.insert(result.end(), packet_header.begin(), packet_header.end());
    result.insert(result.end(), message.begin(), message.end());

    return result;
}

} // namespace cme_mock