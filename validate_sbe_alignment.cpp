#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

// This program validates SBE message alignment by:
// 1. Creating messages with known values
// 2. Checking field offsets
// 3. Verifying byte-by-byte structure
// 4. Generating reference data files for client testing

struct FieldInfo {
    std::string name;
    size_t offset;
    size_t size;
    std::string type;
    uint64_t test_value;
};

void analyze_message_structure()
{
    std::cout << "\n=== Analyzing MDIncrementalRefreshBook46 Structure ===" << std::endl;

    // Create a test message
    std::vector<char> buffer(2048, 0);

    // Encode header
    cme_sbe::MessageHeader header(buffer.data(), 0, buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
        .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    size_t header_size = header.encodedLength();

    std::cout << "\nMessage Header Structure:" << std::endl;
    std::cout << "  Size: " << header_size << " bytes" << std::endl;
    std::cout << "  Offset 0-1: Block Length = " << header.blockLength() << std::endl;
    std::cout << "  Offset 2-3: Template ID = " << header.templateId() << std::endl;
    std::cout << "  Offset 4-5: Schema ID = " << header.schemaId() << std::endl;
    std::cout << "  Offset 6-7: Version = " << static_cast<int>(header.version()) << std::endl;

    // Encode message with specific test values
    cme_sbe::MDIncrementalRefreshBook46 msg(
        buffer.data(),
        header_size,
        buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    // Set test values
    uint64_t test_transact_time = 0x1122334455667788ULL;
    msg.transactTime(test_transact_time);

    msg.matchEventIndicator()
        .lastTradeMsg(true)
        .lastVolumeMsg(false)
        .lastQuoteMsg(true)
        .lastStatsMsg(false)
        .lastImpliedMsg(false)
        .recoveryMsg(false)
        .reserved(false)
        .endOfEvent(true);

    std::cout << "\nMessage Body Structure:" << std::endl;
    std::cout << "  Block Length: " << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << " bytes" << std::endl;

    // Analyze the raw bytes
    std::cout << "\nField Layout (after header at offset " << header_size << "):" << std::endl;

    // TransactTime field
    std::cout << "  TransactTime (uint64):" << std::endl;
    std::cout << "    Offset: 0-7 (relative to message start)" << std::endl;
    std::cout << "    Test value: 0x" << std::hex << test_transact_time << std::dec << std::endl;
    std::cout << "    Actual bytes: ";
    for (int i = 0; i < 8; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (static_cast<int>(buffer[header_size + i]) & 0xFF) << " ";
    }
    std::cout << std::dec << std::endl;

    // MatchEventIndicator field
    std::cout << "  MatchEventIndicator (uint8 bitmap):" << std::endl;
    std::cout << "    Offset: 8 (relative to message start)" << std::endl;
    std::cout << "    Value: 0x" << std::hex
              << static_cast<int>(buffer[header_size + 8]) << std::dec << std::endl;

    // Add group entries to check repeating group structure
    auto& entries = msg.noMDEntriesCount(1);
    entries.next();
    entries.mDEntryPx().mantissa(0x1234567890ABCDEFLL);
    entries.mDEntrySize(0x11223344);
    entries.securityID(0x55667788);
    entries.rptSeq(0x99AABBCC);
    entries.numberOfOrders(0xDDEEFF00);
    entries.mDPriceLevel(0x12);
    entries.mDUpdateAction(cme_sbe::MDUpdateAction::New);
    entries.mDEntryType(cme_sbe::MDEntryTypeBook::Bid);

    size_t total_size = header_size + msg.encodedLength();

    std::cout << "\nRepeating Group Structure:" << std::endl;
    std::cout << "  Group block offset: " << cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength() << std::endl;
    std::cout << "  Number of entries: " << entries.count() << std::endl;
    std::cout << "  Entry size: Check individual field offsets below" << std::endl;

    // Print complete message hex dump
    std::cout << "\nComplete Message Hex Dump (" << total_size << " bytes):" << std::endl;
    for (size_t i = 0; i < total_size; i++) {
        if (i % 16 == 0) {
            if (i > 0)
                std::cout << std::endl;
            std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex
                  << (static_cast<int>(buffer[i]) & 0xFF) << " ";
    }
    std::cout << std::dec << std::endl;
}

void generate_reference_packet()
{
    std::cout << "\n=== Generating Reference Packet ===" << std::endl;

    // Create a complete packet with Binary Packet Header + Message Header + Message
    std::vector<uint8_t> packet;

    // 1. Binary Packet Header (12 bytes)
    uint32_t sequence_number = 0x12345678;
    uint64_t sending_time = 0x1122334455667788ULL;

    // Sequence number (little-endian)
    packet.push_back(sequence_number & 0xFF);
    packet.push_back((sequence_number >> 8) & 0xFF);
    packet.push_back((sequence_number >> 16) & 0xFF);
    packet.push_back((sequence_number >> 24) & 0xFF);

    // Sending time (little-endian)
    for (int i = 0; i < 8; i++) {
        packet.push_back((sending_time >> (i * 8)) & 0xFF);
    }

    // 2. Message Size (2 bytes, little-endian) - placeholder, will update later
    size_t msg_size_offset = packet.size();
    packet.push_back(0);
    packet.push_back(0);

    // 3. SBE Message Header + Body
    std::vector<char> sbe_buffer(1024, 0);

    cme_sbe::MessageHeader header(sbe_buffer.data(), 0, sbe_buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    header.blockLength(cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength())
        .templateId(cme_sbe::MDIncrementalRefreshBook46::sbeTemplateId())
        .schemaId(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaId())
        .version(cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    size_t header_size = header.encodedLength();

    cme_sbe::MDIncrementalRefreshBook46 msg(
        sbe_buffer.data(),
        header_size,
        sbe_buffer.size(),
        cme_sbe::MDIncrementalRefreshBook46::sbeBlockLength(),
        cme_sbe::MDIncrementalRefreshBook46::sbeSchemaVersion());

    msg.transactTime(0xAABBCCDDEEFF0011ULL);
    msg.matchEventIndicator()
        .lastTradeMsg(true)
        .lastVolumeMsg(true)
        .lastQuoteMsg(true)
        .lastStatsMsg(false)
        .lastImpliedMsg(false)
        .recoveryMsg(false)
        .reserved(false)
        .endOfEvent(true);

    // Add one entry
    auto& entries = msg.noMDEntriesCount(1);
    entries.next();
    entries.mDEntryPx().mantissa(2850000000000LL); // 285.00
    entries.mDEntrySize(100);
    entries.securityID(123456);
    entries.rptSeq(1);
    entries.numberOfOrders(5);
    entries.mDPriceLevel(1);
    entries.mDUpdateAction(cme_sbe::MDUpdateAction::New);
    entries.mDEntryType(cme_sbe::MDEntryTypeBook::Bid);

    size_t sbe_msg_size = header_size + msg.encodedLength();

    // Update message size field
    packet[msg_size_offset] = sbe_msg_size & 0xFF;
    packet[msg_size_offset + 1] = (sbe_msg_size >> 8) & 0xFF;

    // Append SBE message to packet
    for (size_t i = 0; i < sbe_msg_size; i++) {
        packet.push_back(static_cast<uint8_t>(sbe_buffer[i]));
    }

    std::cout << "Reference packet generated:" << std::endl;
    std::cout << "  Binary Packet Header: 12 bytes" << std::endl;
    std::cout << "  Message Size Field: 2 bytes (value=" << sbe_msg_size << ")" << std::endl;
    std::cout << "  SBE Message: " << sbe_msg_size << " bytes" << std::endl;
    std::cout << "  Total packet size: " << packet.size() << " bytes" << std::endl;

    // Save to file
    std::ofstream outfile("reference_packet.bin", std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(packet.data()), packet.size());
    outfile.close();

    std::cout << "\nReference packet saved to: reference_packet.bin" << std::endl;

    // Print hex dump
    std::cout << "\nPacket Structure Breakdown:" << std::endl;
    std::cout << "Offset 0-3: Sequence Number = 0x" << std::hex << sequence_number << std::dec << std::endl;
    std::cout << "Offset 4-11: Sending Time = 0x" << std::hex << sending_time << std::dec << std::endl;
    std::cout << "Offset 12-13: Message Size = " << sbe_msg_size << std::endl;
    std::cout << "Offset 14+: SBE Message" << std::endl;

    std::cout << "\nComplete Packet Hex Dump:" << std::endl;
    for (size_t i = 0; i < packet.size(); i++) {
        if (i % 16 == 0) {
            if (i > 0)
                std::cout << std::endl;
            std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex
                  << static_cast<int>(packet[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

void check_endianness()
{
    std::cout << "\n=== System Endianness Check ===" << std::endl;

    uint32_t test = 0x12345678;
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&test);

    std::cout << "Test value: 0x12345678" << std::endl;
    std::cout << "Memory layout: ";
    for (int i = 0; i < 4; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(bytes[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    if (bytes[0] == 0x78) {
        std::cout << "System is: LITTLE-ENDIAN" << std::endl;
        std::cout << "CME MDP uses: LITTLE-ENDIAN" << std::endl;
        std::cout << "Status: COMPATIBLE ✓" << std::endl;
    } else {
        std::cout << "System is: BIG-ENDIAN" << std::endl;
        std::cout << "CME MDP uses: LITTLE-ENDIAN" << std::endl;
        std::cout << "Status: Byte swapping required!" << std::endl;
    }
}

int main()
{
    std::cout << "SBE Alignment Validation Tool" << std::endl;
    std::cout << "=============================" << std::endl;

    check_endianness();
    analyze_message_structure();
    generate_reference_packet();

    std::cout << "\n=== Validation Complete ===" << std::endl;
    std::cout << "Use reference_packet.bin to test your client decoder" << std::endl;
    std::cout << "Compare the hex dumps above with your client's interpretation" << std::endl;

    return 0;
}