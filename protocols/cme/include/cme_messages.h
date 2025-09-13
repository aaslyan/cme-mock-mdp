#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cme_protocol {

// CME MDP 3.0 Message Types
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
    MDInstrumentDefinitionFuture = 54,
    MDInstrumentDefinitionOption = 55,
    MDInstrumentDefinitionSpread = 56,
    SecurityStatus = 30
};

// CME MDP header
struct MDPHeader {
    uint32_t sequence_number;
    uint64_t sending_time;
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
    TradeVolume = 'B',
    OpenInterest = 'C',
    ImpliedBid = 'E',
    ImpliedOffer = 'F',
    BookReset = 'J',
    SessionHighBid = 'N',
    SessionLowOffer = 'O'
};

// Update Action
enum class MDUpdateAction : uint8_t {
    New = 0,
    Change = 1,
    Delete = 2,
    DeleteThru = 3,
    DeleteFrom = 4,
    Overlay = 5
};

// Security Update Action
enum class SecurityUpdateAction : uint8_t {
    Add = 'A',
    Delete = 'D',
    Modify = 'M'
};

// Match Event Indicator
struct MatchEventIndicator {
    bool end_of_event : 1;
    bool reserved : 1;
    bool recovery_msg : 1;
    bool last_implied_msg : 1;
    bool last_stats_msg : 1;
    bool last_quote_msg : 1;
    bool last_volume_msg : 1;
    bool last_trade_msg : 1;

    uint8_t to_byte() const
    {
        return (end_of_event ? 0x80 : 0)
            | (recovery_msg ? 0x20 : 0)
            | (last_implied_msg ? 0x10 : 0)
            | (last_stats_msg ? 0x08 : 0)
            | (last_quote_msg ? 0x04 : 0)
            | (last_volume_msg ? 0x02 : 0)
            | (last_trade_msg ? 0x01 : 0);
    }
};

// CME Price Level Entry
struct MDPriceLevel {
    MDUpdateAction update_action;
    MDEntryType entry_type;
    uint32_t security_id;
    uint32_t rpt_seq;
    int64_t price; // Price * 10^price_decimals
    int32_t quantity;
    uint32_t number_of_orders;
    uint8_t price_level;
    int32_t tradeable_size;
};

// CME Trade Entry
struct MDTrade {
    uint32_t security_id;
    uint32_t rpt_seq;
    int64_t price;
    int32_t quantity;
    uint32_t number_of_orders;
    int8_t aggressor_side;
    std::string trade_id;
};

// CME Statistics Entry
struct MDStatistics {
    uint32_t security_id;
    uint32_t rpt_seq;
    int64_t price;
    int32_t quantity;
    MDEntryType stat_type;
    MDUpdateAction update_action;
};

// Incremental Refresh message
struct IncrementalRefresh {
    uint64_t transact_time;
    MatchEventIndicator match_event_indicator;
    std::vector<MDPriceLevel> entries;
};

// Snapshot Full Refresh message
struct SnapshotFullRefresh {
    uint32_t security_id;
    uint64_t transact_time;
    uint32_t rpt_seq;
    std::vector<MDPriceLevel> bid_entries;
    std::vector<MDPriceLevel> ask_entries;
    double last_trade_price;
    uint64_t total_volume;
};

// Security Definition
struct SecurityDefinition {
    uint32_t security_id;
    std::string symbol;
    std::string security_group;
    std::string asset;
    SecurityUpdateAction update_action;
    int64_t min_price_increment;
    int32_t display_factor;
    std::string maturity_month_year;
    std::string cfi_code;
    std::string currency;
    std::string security_exchange;
};

} // namespace cme_protocol