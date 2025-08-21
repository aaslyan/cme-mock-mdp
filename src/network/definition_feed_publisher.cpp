#include "network/definition_feed_publisher.h"
#include "messages/cme_sbe_encoder.h"
#include "messages/message_factory.h"
#include "utils/logger.h"
#include <sstream>
#include <iostream>
#include <iomanip>

namespace cme_mock {

DefinitionFeedPublisher::DefinitionFeedPublisher(const std::string& ip, uint16_t port,
                                               std::shared_ptr<ReferenceDataManager> ref_data_manager)
    : FeedPublisher(ip, port, nullptr) // No order book manager needed
    , ref_data_manager_(ref_data_manager)
    , sequence_number_(1)
{
}

void DefinitionFeedPublisher::publish_all_definitions()
{
    auto instruments = ref_data_manager_->get_all_instruments();

    LOG_INFO("Publishing instrument definitions for " + std::to_string(instruments.size()) + " instruments");

    for (const auto* instrument : instruments) {
        if (instrument) {
            publish_instrument_definition(instrument->security_id);
        }
    }
}

void DefinitionFeedPublisher::publish_instrument_definition(uint32_t security_id)
{
    const auto* instrument = ref_data_manager_->get_instrument(security_id);
    if (!instrument) {
        LOG_WARNING("Instrument not found for security ID " + std::to_string(security_id));
        return;
    }

    // Encode and send
    auto encoded = encode_definition(*instrument);

    // Print hex data for debugging
    std::cout << "SERVER SENDING DEFINITION (" << encoded.size() << " bytes) for " 
              << instrument->symbol << ": ";
    for (size_t i = 0; i < std::min(encoded.size(), size_t(32)); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (unsigned)encoded[i] << " ";
    }
    std::cout << std::dec << std::endl;

    if (udp_publisher_->send(encoded)) {
        std::stringstream ss;
        ss << "Sent definition for " << instrument->symbol
           << " (ID: " << security_id << ")"
           << " - " << encoded.size() << " bytes";
        LOG_DEBUG(ss.str());
    } else {
        LOG_ERROR("Failed to send definition for security " + std::to_string(security_id));
    }
}

void DefinitionFeedPublisher::publish_instrument_definition(const std::string& symbol)
{
    const auto* instrument = ref_data_manager_->get_instrument(symbol);
    if (!instrument) {
        LOG_WARNING("Instrument not found for symbol " + symbol);
        return;
    }

    publish_instrument_definition(instrument->security_id);
}

std::vector<uint8_t> DefinitionFeedPublisher::encode_definition(const InstrumentDefinition& instrument)
{
    // Encode complete MDP packet with proper CME format
    auto packet_header = CMESBEEncoder::encode_packet_header(
        sequence_number_++,
        get_timestamp_ns());

    auto message = InstrumentEncoder::encode_instrument_definition(instrument);

    // Build complete packet: Binary Packet Header + Message Size + SBE Message
    std::vector<uint8_t> result;
    result.reserve(packet_header.size() + 2 + message.size());

    // 1. Add Binary Packet Header (12 bytes)
    result.insert(result.end(), packet_header.begin(), packet_header.end());

    // 2. Add Message Size (2 bytes, little-endian) - total message length
    uint16_t message_size = static_cast<uint16_t>(message.size());
    result.push_back(message_size & 0xFF);
    result.push_back((message_size >> 8) & 0xFF);

    // 3. Add SBE Message (contains Block Length + Template ID + Schema ID + Version + Body)
    result.insert(result.end(), message.begin(), message.end());

    return result;
}

} // namespace cme_mock