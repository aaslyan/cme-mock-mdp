#include "cme_sbe/MessageHeader.h"
#include "messages/cme_sbe_encoder.h"
#include <iomanip>
#include <iostream>
#include <vector>

using namespace cme_mock;

void printHex(const std::vector<uint8_t>& buffer, size_t length)
{
    std::cout << "Hex: ";
    for (size_t i = 0; i < std::min(length, size_t(32)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (unsigned char)buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

void analyzeHeader(const std::vector<uint8_t>& buffer)
{
    if (buffer.size() < 8) {
        std::cout << "Buffer too small for header analysis" << std::endl;
        return;
    }

    // Parse header manually
    uint16_t blockLength = (buffer[1] << 8) | buffer[0];
    uint16_t templateId = (buffer[3] << 8) | buffer[2];
    uint16_t schemaId = (buffer[5] << 8) | buffer[4];
    uint16_t version = (buffer[7] << 8) | buffer[6];

    std::cout << "Parsed Header:" << std::endl;
    std::cout << "  Block Length: " << blockLength << std::endl;
    std::cout << "  Template ID: " << templateId << std::endl;
    std::cout << "  Schema ID: " << schemaId << std::endl;
    std::cout << "  Version: " << version << std::endl;
}

int main()
{
    std::cout << "Testing CME SBE Encoder" << std::endl;

    try {
        // Test incremental refresh
        std::cout << "\n=== Incremental Refresh ===\n";
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
        std::cout << "Encoded length: " << inc_buffer.size() << " bytes" << std::endl;
        printHex(inc_buffer, inc_buffer.size());
        analyzeHeader(inc_buffer);

        // Test snapshot refresh
        std::cout << "\n=== Snapshot Refresh ===\n";
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
        std::cout << "Encoded length: " << snap_buffer.size() << " bytes" << std::endl;
        printHex(snap_buffer, snap_buffer.size());
        analyzeHeader(snap_buffer);

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}