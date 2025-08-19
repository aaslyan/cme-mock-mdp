#include "messages/cme_sbe_encoder.h"
#include "messages/sbe_decoder.h"
#include "utils/logger.h"
#include <iostream>
#include <iomanip>

using namespace cme_mock;

void analyze_sbe_message(const std::vector<uint8_t>& data, const std::string& type)
{
    std::cout << "\n=== " << type << " Analysis (" << data.size() << " bytes) ===" << std::endl;

    // Print hex dump
    std::cout << "Hex dump: ";
    for (size_t i = 0; i < std::min(data.size(), size_t(32)); ++i) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    SBEDecoder decoder(data);

    // Try to decode message header directly (skip packet header)
    uint16_t block_length, schema_id, version;
    uint16_t template_id = MDPMessageDecoder::decode_message_header(
        decoder, block_length, schema_id, version);

    std::cout << "Message Header - Template: " << template_id
              << ", Schema: " << schema_id << ", Version: " << version
              << ", Block Length: " << block_length << std::endl;

    // Identify message type
    if (template_id == 52) {
        std::cout << "Message Type: SNAPSHOT_FULL_REFRESH (Template 52)" << std::endl;
    } else if (template_id == 46) {
        std::cout << "Message Type: INCREMENTAL_REFRESH_BOOK (Template 46)" << std::endl;
    } else {
        std::cout << "Message Type: UNKNOWN (ID: " << template_id << ")" << std::endl;
    }
}

int main()
{
    std::cout << "Testing CME SBE Encoder with SBE Decoder" << std::endl;

    Logger::instance().set_level(LogLevel::INFO);

    try {
        // Test incremental refresh
        std::cout << "\n=== Creating Incremental Refresh ===\n";
        IncrementalRefresh inc;
        inc.transact_time = 123456789;
        
        MDPriceLevel level;
        level.update_action = MDUpdateAction::New;
        level.entry_type = MDEntryType::Bid;
        level.security_id = 31;
        level.rpt_seq = 1;
        level.price = 10000;
        level.quantity = 100;
        level.number_of_orders = 1;
        level.price_level = 1;
        inc.price_levels.push_back(level);
        
        auto inc_buffer = CMESBEEncoder::encode_incremental_refresh(inc);
        analyze_sbe_message(inc_buffer, "Incremental Refresh");
        
        // Test snapshot refresh
        std::cout << "\n=== Creating Snapshot Refresh ===\n";
        SnapshotFullRefresh snapshot;
        snapshot.security_id = 31;
        snapshot.last_msg_seq_num_processed = 100;
        snapshot.rpt_seq = 1;
        snapshot.tot_num_reports = 1;
        snapshot.transact_time = 123456789;
        
        MDSnapshotEntry entry;
        entry.entry_type = MDEntryType::Bid;
        entry.price = 10000;
        entry.quantity = 100;
        entry.number_of_orders = 1;
        entry.price_level = 1;
        snapshot.entries.push_back(entry);
        
        auto snap_buffer = CMESBEEncoder::encode_snapshot_full_refresh(snapshot);
        analyze_sbe_message(snap_buffer, "Snapshot Refresh");
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}