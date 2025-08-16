#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cme_mock {

// MDP 3.0 Message Types
enum class MDPMessageType : uint16_t {
    // Administrative Messages
    ChannelReset = 4,
    AdminHeartbeat = 12,

    // Market Data Messages
    IncrementalRefreshBook = 46,
    IncrementalRefreshTrade = 48,
    IncrementalRefreshVolume = 37,
    IncrementalRefreshSessionStatistics = 51,

    // Snapshot Messages
    SnapshotFullRefresh = 52,

    // Security Definition
    SecurityDefinition = 39,
    SecurityStatus = 30
};

// Common MDP header
struct MDPHeader {
    uint32_t sequence_number;
    uint64_t sending_time;
    uint16_t msg_count;
};

// Market Data Entry Types
enum class MDEntryType : char {
    Bid = '0',
    Offer = '1',
    Trade = '2',
    OpeningPrice = '4',
    ClosingPrice = '5',
    SettlementPrice = '6',
    TradingSessionHighPrice = '7',
    TradingSessionLowPrice = '8',
    TradeVolume = 'B'
};

// Update Action
enum class MDUpdateAction : uint8_t {
    New = 0,
    Change = 1,
    Delete = 2,
    Overlay = 5
};

// Price Level Entry
struct MDPriceLevel {
    MDUpdateAction update_action;
    MDEntryType entry_type;
    uint32_t security_id;
    uint32_t rpt_seq;
    int64_t price; // Scaled price (price * 10^price_decimals)
    int32_t quantity;
    uint32_t number_of_orders;
    uint8_t price_level;
};

// Trade Entry
struct MDTrade {
    uint32_t security_id;
    uint32_t rpt_seq;
    int64_t price;
    int32_t quantity;
    uint32_t number_of_orders;
    char aggressor_side;
};

// Snapshot Entry
struct MDSnapshotEntry {
    MDEntryType entry_type;
    int64_t price;
    int32_t quantity;
    uint32_t number_of_orders;
    uint8_t price_level;
};

// Full Snapshot Message
struct SnapshotFullRefresh {
    MDPHeader header;
    uint32_t security_id;
    uint32_t last_msg_seq_num_processed;
    uint32_t rpt_seq;
    uint32_t tot_num_reports;
    uint8_t security_trading_status;
    uint64_t transact_time;
    std::vector<MDSnapshotEntry> entries;
};

// Incremental Refresh Message
struct IncrementalRefresh {
    MDPHeader header;
    uint64_t transact_time;
    std::vector<MDPriceLevel> price_levels;
    std::vector<MDTrade> trades;
};

} // namespace cme_mock