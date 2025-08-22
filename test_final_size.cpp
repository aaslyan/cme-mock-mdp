#include "cme_sbe/MDIncrementalRefreshBook46.h"
#include "cme_sbe/MessageHeader.h"
#include "messages/cme_sbe_encoder.h"
#include <iomanip>
#include <iostream>
#include <vector>

using namespace cme_mock;

void printHex(const std::vector<uint8_t>& data, const std::string& label)
{
    std::cout << label << " (" << data.size() << " bytes):" << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";
        if ((i + 1) % 16 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl
              << std::dec;
}

int main()
{
    std::cout << "Testing final message size..." << std::endl;

    // Create incremental refresh
    IncrementalRefresh refresh;
    refresh.transact_time = 1234567890123456789ULL;

    MDPriceLevel level1;
    level1.security_id = 100;
    level1.rpt_seq = 1;
    level1.price = 10025;
    level1.price_level = 1;
    level1.entry_type = MDEntryType::Bid;
    level1.update_action = MDUpdateAction::New;
    refresh.price_levels.push_back(level1);

    // Encode SBE message
    auto sbe_message = CMESBEEncoder::encode_incremental_refresh(refresh);
    std::cout << "SBE message size: " << sbe_message.size() << " bytes" << std::endl;

    // Create complete packet
    std::vector<std::vector<uint8_t>> messages = { sbe_message };
    auto packet = CMESBEEncoder::encode_multi_message_packet(12345, 987654321ULL, messages);

    std::cout << "Complete packet size: " << packet.size() << " bytes" << std::endl;

    // Verify structure
    std::cout << "\nPacket structure:" << std::endl;
    std::cout << "  Packet header: 12 bytes" << std::endl;
    std::cout << "  Message size field: 2 bytes" << std::endl;
    std::cout << "  SBE message: " << sbe_message.size() << " bytes" << std::endl;
    std::cout << "  Expected total: " << (12 + 2 + sbe_message.size()) << " bytes" << std::endl;

    // Check message size field
    uint16_t msg_size = packet[12] | (packet[13] << 8);
    std::cout << "\nMessage size field value: " << msg_size << std::endl;

    if (msg_size == sbe_message.size()) {
        std::cout << "✓ Message size field is CORRECT" << std::endl;
    } else {
        std::cout << "✗ Message size field is WRONG" << std::endl;
        std::cout << "  Expected: " << sbe_message.size() << std::endl;
        std::cout << "  Got: " << msg_size << std::endl;
    }

    // Test decoder
    std::cout << "\nTesting decoder:" << std::endl;
    cme_sbe::MessageHeader header;
    header.wrap(reinterpret_cast<char*>(sbe_message.data()), 0, 0, sbe_message.size());

    cme_sbe::MDIncrementalRefreshBook46 decoder;
    decoder.wrapForDecode(
        reinterpret_cast<char*>(sbe_message.data()),
        header.encodedLength(),
        header.blockLength(),
        header.version(),
        sbe_message.size());

    auto& md_entries = decoder.noMDEntries();
    auto& order_entries = decoder.noOrderIDEntries();

    std::cout << "  NoMDEntries count: " << md_entries.count() << std::endl;
    std::cout << "  NoOrderIDEntries count: " << order_entries.count() << std::endl;
    std::cout << "  Order entries hasNext(): " << (order_entries.hasNext() ? "true" : "false") << std::endl;

    return 0;
}