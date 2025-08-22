#pragma once

#include "cme_sbe/MDInstrumentDefinitionFuture54.h"
#include "cme_sbe/MDInstrumentDefinitionSpread56.h"
#include "cme_sbe/MessageHeader.h"
#include "core/reference_data.h"
#include <cstdint>
#include <vector>

namespace cme_mock {

// CME Instrument Definition Encoder using generated SBE code
class InstrumentEncoder {
public:
    // Encode instrument definition based on type
    static std::vector<uint8_t> encode_instrument_definition(
        const InstrumentDefinition& instrument);

    // Encode Future instrument (Template 54)
    static std::vector<uint8_t> encode_future_definition(
        const InstrumentDefinition& instrument);

    // Encode Spread instrument (Template 56)
    static std::vector<uint8_t> encode_spread_definition(
        const InstrumentDefinition& instrument);

private:
    // Helper to determine instrument type from symbol
    static bool is_spread_instrument(const std::string& symbol);

    // Convert our internal structures to SBE format
    static void populate_future_message(
        cme_sbe::MDInstrumentDefinitionFuture54& sbe_msg,
        const InstrumentDefinition& instrument);

    static void populate_spread_message(
        cme_sbe::MDInstrumentDefinitionSpread56& sbe_msg,
        const InstrumentDefinition& instrument);
};

} // namespace cme_mock