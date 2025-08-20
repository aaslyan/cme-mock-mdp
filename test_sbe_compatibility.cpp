#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include "include/cme_sbe/SnapshotFullRefresh52.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

// Test program to validate SBE encoding/decoding compatibility
// This generates reference messages that can be compared with client decoder

void print_hex_dump(const std::vector<uint8_t>& data, const std::string& label)
{
    std::cout << "\n=== " << label << " ===" << std::endl;
    std::cout << "Size: " << data.size() << " bytes" << std::endl;
    std::cout << "Hex dump:" << std::endl;

    for (size_t i = 0; i < data.size(); i += 16) {
        std::cout << std::setfill('0') << std::setw(4) << std::hex << i << ": ";

        // Print hex values
        for (size_t j = 0; j < 16 && (i + j) < data.size(); ++j) {
            std::cout << std::setfill('0') << std::setw(2) << std::hex
                      << static_cast<int>(data[i + j]) << " ";
        }

        // Print ASCII representation
        std::cout << " |";
        for (size_t j = 0; j < 16 && (i + j) < data.size(); ++j) {
            char c = static_cast<char>(data[i + j]);
            std::cout << (isprint(c) ? c : '.');
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::dec;
}

void test_message_header()
{
    std::cout << "\n========== Testing Message Header ==========" << std::endl;

    // Create buffer for header
    std::vector<char> buffer(256, 0);

    // Encode header for MDIncrementalRefreshBook46
    cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
        .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    std::cout << "Header encoded length: " << header.encodedLength() << " bytes" << std::endl;
    std::cout << "Block Length: " << header.blockLength() << std::endl;
    std::cout << "Template ID: " << header.templateId() << std::endl;
    std::cout << "Schema ID: " << header.schemaId() << std::endl;
    std::cout << "Version: " << static_cast<int>(header.version()) << std::endl;

    // Print raw header bytes
    std::vector<uint8_t> header_bytes(buffer.begin(), buffer.begin() + header.encodedLength());
    print_hex_dump(header_bytes, "Message Header");
}

void test_incremental_refresh()
{
    std::cout << "\n========== Testing MDIncrementalRefreshBook46 ==========" << std::endl;

    // Create buffer
    std::vector<char> buffer(2048, 0);

    // Step 1: Encode header
    cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
        .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    size_t header_size = header.encodedLength();

    // Step 2: Encode message after header
    cme_sbe::MDIncrementalRefreshBook46 msg(
        buffer.data(),
        header_size,
        buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    // Set fields
    msg.transactTime(1234567890123456789ULL);
    msg.matchEventIndicator()
        .lastTradeMsg(true)
        .lastVolumeMsg(false)
        .lastQuoteMsg(true)
        .lastStatsMsg(false)
        .lastImpliedMsg(false)
        .recoveryMsg(false)
        .reserved(false)
        .endOfEvent(true);

    // Add entries
    auto& entries = msg.noMDEntriesCount(2);

    // Entry 1
    entries.next()
        .mDEntryPx()
        .mantissa(2850000000000LL); // Price 285.00
    entries.mDEntrySize(100);
    entries.securityID(123456);
    entries.rptSeq(1);
    entries.numberOfOrders(5);
    entries.mDPriceLevel(1);
    entries.mDUpdateAction(cme_sbe::MDUpdateAction::New);
    entries.mDEntryType(cme_sbe::MDEntryTypeBook::Bid);

    // Entry 2
    entries.next()
        .mDEntryPx()
        .mantissa(2851000000000LL); // Price 285.10
    entries.mDEntrySize(200);
    entries.securityID(123456);
    entries.rptSeq(2);
    entries.numberOfOrders(3);
    entries.mDPriceLevel(1);
    entries.mDUpdateAction(cme_sbe::MDUpdateAction::New);
    entries.mDEntryType(cme_sbe::MDEntryTypeBook::Offer);

    // Calculate total size
    size_t total_size = header_size + msg.encodedLength();

    std::cout << "Message Statistics:" << std::endl;
    std::cout << "  Header size: " << header_size << " bytes" << std::endl;
    std::cout << "  Message body size: " << msg.encodedLength() << " bytes" << std::endl;
    std::cout << "  Total size: " << total_size << " bytes" << std::endl;
    std::cout << "  Number of entries: " << entries.count() << std::endl;

    // Print full message
    std::vector<uint8_t> message_bytes(buffer.begin(), buffer.begin() + total_size);
    print_hex_dump(message_bytes, "Complete MDIncrementalRefreshBook46 Message");

    // Decode and verify
    std::cout << "\n--- Decoding verification ---" << std::endl;

    // Decode header
    cme_sbe::MessageHeader decode_header(buffer.data(), 0, buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    std::cout << "Decoded header:" << std::endl;
    std::cout << "  Template ID: " << decode_header.templateId() << std::endl;
    std::cout << "  Block Length: " << decode_header.blockLength() << std::endl;

    // Decode message
    cme_sbe::MDIncrementalRefreshBook46 decode_msg(
        buffer.data(),
        header_size,
        decode_header.blockLength(),
        total_size - header_size,
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    std::cout << "Decoded message:" << std::endl;
    std::cout << "  TransactTime: " << decode_msg.transactTime() << std::endl;

    auto& decode_entries = decode_msg.noMDEntries();
    std::cout << "  Number of entries: " << decode_entries.count() << std::endl;

    while (decode_entries.hasNext()) {
        decode_entries.next();
        std::cout << "  Entry:" << std::endl;
        std::cout << "    Price: " << decode_entries.mDEntryPx().mantissa() << std::endl;
        std::cout << "    Size: " << decode_entries.mDEntrySize() << std::endl;
        std::cout << "    Security ID: " << decode_entries.securityID() << std::endl;
    }
}

void test_snapshot_full_refresh()
{
    std::cout << "\n========== Testing SnapshotFullRefresh52 ==========" << std::endl;

    // Create buffer
    std::vector<char> buffer(2048, 0);

    // Encode header
    cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(),
        cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());

    header.blockLength(cme_sbe::SnapshotFullRefresh52::sbeBlockLength())
        .templateId(cme_sbe::SnapshotFullRefresh52::sbeTemplateId())
        .schemaId(cme_sbe::SnapshotFullRefresh52::sbeSchemaId())
        .version(cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());

    size_t header_size = header.encodedLength();

    // Encode message
    cme_sbe::SnapshotFullRefresh52 msg(
        buffer.data(),
        header_size,
        buffer.size(),
        cme_sbe::SnapshotFullRefresh52::sbeBlockLength(),
        cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion());

    // Set fields
    msg.lastMsgSeqNumProcessed(1000);
    msg.totNumReports(1);
    msg.securityID(123456);
    msg.rptSeq(1);
    msg.transactTime(1234567890123456789ULL);
    msg.lastUpdateTime(1234567890123456789ULL);
    msg.tradeDate(20241219);

    // Add entries
    auto& entries = msg.noMDEntriesCount(2);

    // Bid entry
    entries.next()
        .mDEntryPx()
        .mantissa(2850000000000LL);
    entries.mDEntrySize(100);
    entries.numberOfOrders(5);
    entries.mDPriceLevel(1);
    entries.tradingReferenceDate(20241219);
    entries.openCloseSettlFlag(cme_sbe::OpenCloseSettlFlag::DailyOpenPrice);
    entries.settlPriceType()
        .final_(false)
        .actual(true)
        .rounded(false)
        .intraday(false)
        .reserved(false)
        .nullValue(false);
    entries.mDEntryType(cme_sbe::MDEntryType::Bid);

    // Ask entry
    entries.next()
        .mDEntryPx()
        .mantissa(2851000000000LL);
    entries.mDEntrySize(200);
    entries.numberOfOrders(3);
    entries.mDPriceLevel(1);
    entries.tradingReferenceDate(20241219);
    entries.openCloseSettlFlag(cme_sbe::OpenCloseSettlFlag::DailyOpenPrice);
    entries.settlPriceType()
        .final_(false)
        .actual(true)
        .rounded(false)
        .intraday(false)
        .reserved(false)
        .nullValue(false);
    entries.mDEntryType(cme_sbe::MDEntryType::Offer);

    size_t total_size = header_size + msg.encodedLength();

    std::cout << "Message Statistics:" << std::endl;
    std::cout << "  Header size: " << header_size << " bytes" << std::endl;
    std::cout << "  Message body size: " << msg.encodedLength() << " bytes" << std::endl;
    std::cout << "  Total size: " << total_size << " bytes" << std::endl;

    std::vector<uint8_t> message_bytes(buffer.begin(), buffer.begin() + total_size);
    print_hex_dump(message_bytes, "Complete SnapshotFullRefresh52 Message");
}

void test_alignment_and_padding()
{
    std::cout << "\n========== Testing Alignment and Padding ==========" << std::endl;

    // Test different field alignments
    std::cout << "\nSBE Type Sizes and Alignments:" << std::endl;
    std::cout << "  MessageHeader size: " << sizeof(cme_sbe::MessageHeader) << " bytes" << std::endl;
    std::cout << "  MDIncrementalRefreshBook46 block length: "
              << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << " bytes" << std::endl;
    std::cout << "  SnapshotFullRefresh52 block length: "
              << cme_sbe::SnapshotFullRefresh52::sbeBlockLength() << " bytes" << std::endl;

    // Test schema constants
    std::cout << "\nSchema Constants:" << std::endl;
    std::cout << "  MDIncrementalRefreshBook46:" << std::endl;
    std::cout << "    Template ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId() << std::endl;
    std::cout << "    Schema ID: " << cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId() << std::endl;
    std::cout << "    Schema Version: " << static_cast<int>(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion()) << std::endl;

    std::cout << "  SnapshotFullRefresh52:" << std::endl;
    std::cout << "    Template ID: " << cme_sbe::SnapshotFullRefresh52::sbeTemplateId() << std::endl;
    std::cout << "    Schema ID: " << cme_sbe::SnapshotFullRefresh52::sbeSchemaId() << std::endl;
    std::cout << "    Schema Version: " << static_cast<int>(cme_sbe::SnapshotFullRefresh52::sbeSchemaVersion()) << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "CME SBE Compatibility Test Suite" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "This tool generates reference SBE messages for client validation" << std::endl;

    test_message_header();
    test_incremental_refresh();
    test_snapshot_full_refresh();
    test_alignment_and_padding();

    std::cout << "\n========== Test Complete ==========" << std::endl;
    std::cout << "Use the hex dumps above to verify client decoder compatibility" << std::endl;

    return 0;
}