#pragma once

#include "network/feed_publisher.h"
#include "messages/instrument_encoder.h"
#include "core/reference_data.h"
#include <memory>

namespace cme_mock {

// Publisher for instrument definition messages (Templates 54, 56, etc.)
class DefinitionFeedPublisher : public FeedPublisher {
public:
    DefinitionFeedPublisher(const std::string& ip, uint16_t port,
                           std::shared_ptr<ReferenceDataManager> ref_data_manager);

    // Publish all instrument definitions at session start
    void publish_all_definitions();

    // Publish definition for specific instrument
    void publish_instrument_definition(uint32_t security_id);

    // Publish definition by symbol
    void publish_instrument_definition(const std::string& symbol);

private:
    std::shared_ptr<ReferenceDataManager> ref_data_manager_;
    uint32_t sequence_number_;

    // Encode instrument definition with proper packet structure
    std::vector<uint8_t> encode_definition(const InstrumentDefinition& instrument);
};

} // namespace cme_mock