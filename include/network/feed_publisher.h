#pragma once

#include <memory>
#include <chrono>
#include "udp_publisher.h"
#include "core/order_book_manager.h"
#include "messages/mdp_messages.h"

namespace cme_mock {

// Base class for feed publishers
class FeedPublisher {
public:
    FeedPublisher(const std::string& ip, uint16_t port, 
                  std::shared_ptr<OrderBookManager> book_manager);
    virtual ~FeedPublisher() = default;
    
    bool initialize();
    
protected:
    std::shared_ptr<UDPPublisher> udp_publisher_;
    std::shared_ptr<OrderBookManager> book_manager_;
};

// Snapshot feed publisher
class SnapshotFeedPublisher : public FeedPublisher {
public:
    using FeedPublisher::FeedPublisher;
    
    // Publish snapshots for all instruments
    void publish_all_snapshots();
    
    // Publish snapshot for specific instrument
    void publish_snapshot(uint32_t security_id);
    
    uint32_t get_sequence() const { return snapshot_sequence_; }
    
private:
    uint32_t snapshot_sequence_ = 1;
    
    std::vector<uint8_t> encode_snapshot(const SnapshotFullRefresh& snapshot);
};

// Incremental feed publisher
class IncrementalFeedPublisher : public FeedPublisher {
public:
    using FeedPublisher::FeedPublisher;
    
    // Publish incremental updates
    void publish_book_update(uint32_t security_id, const MDPriceLevel& level);
    void publish_trade(uint32_t security_id, const MDTrade& trade);
    void publish_batch(const IncrementalRefresh& update);
    
    uint32_t get_sequence() const { return sequence_number_; }
    void set_sequence(uint32_t seq) { sequence_number_ = seq; }
    
private:
    uint32_t sequence_number_ = 1;
    
    std::vector<uint8_t> encode_incremental(const IncrementalRefresh& update);
};

} // namespace cme_mock