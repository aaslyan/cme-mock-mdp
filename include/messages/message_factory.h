#pragma once

#include "mdp_messages.h"
#include "core/order_book.h"

namespace cme_mock {

// Helper functions for creating MDP messages
uint64_t get_timestamp_ns();

SnapshotFullRefresh create_snapshot_from_book(
    const OrderBook& book,
    uint32_t last_seq_num,
    uint32_t rpt_seq);

IncrementalRefresh create_price_level_update(
    uint32_t security_id,
    MDUpdateAction action,
    MDEntryType side,
    double price,
    int32_t quantity,
    uint8_t level,
    uint32_t rpt_seq);

IncrementalRefresh create_trade_update(
    uint32_t security_id,
    double price,
    int32_t quantity,
    char aggressor_side,
    uint32_t rpt_seq);

} // namespace cme_mock