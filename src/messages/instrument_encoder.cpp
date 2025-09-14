#include "messages/instrument_encoder.h"
#include "utils/logger.h"
#include <chrono>
#include <cstring>

namespace cme_mock {

bool InstrumentEncoder::is_spread_instrument(const std::string& /* symbol */)
{
    // For now, treat all as futures - simplified approach
    return false;
}

std::vector<uint8_t> InstrumentEncoder::encode_instrument_definition(
    const InstrumentDefinition& instrument)
{
    // Always encode as future for simplicity
    return encode_future_definition(instrument);
}

std::vector<uint8_t> InstrumentEncoder::encode_future_definition(
    const InstrumentDefinition& instrument)
{
    // Calculate buffer size - make it large enough
    size_t buffer_size = 4096;
    std::vector<char> buffer(buffer_size);

    try {
        // Step 1: Wrap and encode the message header
        cme_sbe::MessageHeader header;
        header.wrap(buffer.data(), 0, 0, buffer.size())
            .blockLength(cme_sbe::MDInstrumentDefinitionFuture54::sbeBlockLength())
            .templateId(cme_sbe::MDInstrumentDefinitionFuture54::sbeTemplateId())
            .schemaId(cme_sbe::MDInstrumentDefinitionFuture54::sbeSchemaId())
            .version(cme_sbe::MDInstrumentDefinitionFuture54::sbeSchemaVersion());

        // Step 2: Wrap for encode and populate the message body
        size_t header_size = header.encodedLength();
        cme_sbe::MDInstrumentDefinitionFuture54 sbe_msg;
        sbe_msg.wrapForEncode(buffer.data(), header_size, buffer.size());

        populate_future_message(sbe_msg, instrument);

        // Get the total encoded length (header + message)
        size_t total_length = header_size + sbe_msg.encodedLength();
        buffer.resize(total_length);

        // Convert char buffer to uint8_t buffer
        return std::vector<uint8_t>(buffer.begin(), buffer.end());

    } catch (const std::exception& e) {
        LOG_ERROR("SBE future definition encoding error: " + std::string(e.what()));
        // Fall back to minimal message
        std::vector<uint8_t> minimal_buffer(128, 0);
        return minimal_buffer;
    }
}

std::vector<uint8_t> InstrumentEncoder::encode_spread_definition(
    const InstrumentDefinition& instrument)
{
    // For simplicity, just encode as future
    return encode_future_definition(instrument);
}

void InstrumentEncoder::populate_future_message(
    cme_sbe::MDInstrumentDefinitionFuture54& sbe_msg,
    const InstrumentDefinition& instrument)
{
    // Set basic instrument fields - only the essential ones
    sbe_msg.securityID(instrument.security_id);
    sbe_msg.totNumReports(1);

    // Use enum value instead of char
    sbe_msg.securityUpdateAction(cme_sbe::SecurityUpdateAction::Value::Add);

    sbe_msg.lastUpdateTime(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                               .count());

    // Set symbol - simplified
    sbe_msg.putSymbol(instrument.symbol.c_str());

    // Set security group
    std::string group = instrument.symbol.substr(0, 2);
    sbe_msg.putSecurityGroup(group.c_str());

    // Set asset
    sbe_msg.putAsset(group.c_str());

    // Set security type
    sbe_msg.putSecurityType(std::string("FUT"));

    // Set CFI code
    sbe_msg.putCFICode("FFIXSX");

    // Set currency
    sbe_msg.putCurrency("USD");

    // Set minimal required fields with default values
    sbe_msg.putUnitOfMeasure(std::string("BBL")); // Barrel unit for futures
    sbe_msg.minPriceIncrement().mantissa(250000000); // 0.25 scaled
    sbe_msg.displayFactor().mantissa(1000000000);
    sbe_msg.mainFraction(4);
    sbe_msg.subFraction(0);
    sbe_msg.priceDisplayFormat(2);
    // settlPriceType is a composite - set fields individually
    sbe_msg.settlPriceType().clear();
    sbe_msg.clearedVolume(1);
    sbe_msg.openInterestQty(0); // Use openInterestQty instead

    // Initialize groups as empty
    sbe_msg.noEventsCount(0);
    sbe_msg.noMDFeedTypesCount(0);
    sbe_msg.noInstAttribCount(0);
    sbe_msg.noLotTypeRulesCount(0);
}

void InstrumentEncoder::populate_spread_message(
    cme_sbe::MDInstrumentDefinitionSpread56& sbe_msg,
    const InstrumentDefinition& instrument)
{
    // Not implemented - fallback to future encoding
    (void)sbe_msg;
    (void)instrument;
}

} // namespace cme_mock