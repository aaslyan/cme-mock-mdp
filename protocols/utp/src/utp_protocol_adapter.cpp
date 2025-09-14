#include "../include/utp_protocol_adapter.h"
#include <cstring>
#include <iostream>

namespace utp_protocol {

UTPProtocolAdapter::UTPProtocolAdapter()
    : m_channel_id(0)
    , m_batch_size(1)
    , m_sequence_number(0)
{
    m_send_buffer.reserve(4096);
}

void UTPProtocolAdapter::set_transport(std::shared_ptr<market_protocols::IMessageTransport> transport)
{
    m_transport = transport;
}

void UTPProtocolAdapter::process_quote_event(
    const market_core::Instrument& instrument,
    const market_core::QuoteEvent& event)
{
    (void)instrument; // Suppress unused parameter warning
    m_send_buffer.clear();
    if (create_incremental_from_quote(event, m_send_buffer)) {
        send_utp_message(m_send_buffer);
    }
}

void UTPProtocolAdapter::process_trade_event(
    const market_core::Instrument& instrument,
    const market_core::TradeEvent& event)
{
    (void)instrument; // Suppress unused parameter warning  
    (void)event; // Suppress unused parameter warning
    // UTP doesn't currently support trade messages in this simplified implementation
    // In a full implementation, you would create trade-specific messages
}

void UTPProtocolAdapter::process_snapshot_event(
    const market_core::Instrument& instrument,
    const market_core::SnapshotEvent& event)
{
    (void)instrument; // Suppress unused parameter warning
    m_send_buffer.clear();
    if (create_full_refresh_from_snapshot(event, m_send_buffer)) {
        send_utp_message(m_send_buffer);
    }
}

void UTPProtocolAdapter::process_statistics_event(
    const market_core::Instrument& instrument,
    const market_core::StatisticsEvent& event)
{
    (void)instrument; // Suppress unused parameter warning
    (void)event; // Suppress unused parameter warning
    // UTP doesn't support statistics in this simplified implementation
}

void UTPProtocolAdapter::process_status_event(
    const market_core::Instrument& instrument,
    const market_core::StatusEvent& event)
{
    (void)instrument; // Suppress unused parameter warning
    (void)event; // Suppress unused parameter warning
    // UTP doesn't support status events in this simplified implementation
}

void UTPProtocolAdapter::send_instrument_definition(const market_core::Instrument& instrument)
{
    m_send_buffer.clear();
    if (create_security_definition(instrument, m_send_buffer)) {
        send_utp_message(m_send_buffer);
    }
}

bool UTPProtocolAdapter::create_heartbeat_message(std::vector<uint8_t>& buffer)
{
    try {
        // Heartbeat is just a header - template ID 10 with no body
        size_t header_size = 8; // MessageHeader size
        buffer.resize(header_size);
        
        utp_sbe::MessageHeader header;
        header.wrap(reinterpret_cast<char*>(buffer.data()), 0, header.sbeSchemaVersion(), buffer.size());
        encode_message_header(header, 10, 0); // Template ID 10, no additional fields
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating heartbeat message: " << e.what() << std::endl;
        return false;
    }
}

bool UTPProtocolAdapter::create_security_definition(const market_core::Instrument& instrument, std::vector<uint8_t>& buffer)
{
    try {
        // Create security definition message
        utp_sbe::SecurityDefinition secDef;
        
        // Calculate buffer size needed
        size_t required_size = utp_sbe::MessageHeader::encodedLength() + secDef.sbeBlockLength();
        buffer.resize(required_size);
        
        // Encode message header first
        utp_sbe::MessageHeader header;
        header.wrap(reinterpret_cast<char*>(buffer.data()), 0, header.sbeSchemaVersion(), buffer.size());
        encode_message_header(header, 18, secDef.sbeBlockLength()); // Template ID 18
        
        // Now wrap the security definition after the header
        secDef.wrapForEncode(
            reinterpret_cast<char*>(buffer.data()), 
            header.encodedLength(),
            buffer.size() - header.encodedLength()
        );
        
        // Set security definition fields
        secDef.securityUpdateAction(utp_sbe::SecurityUpdateAction::ADD);
        secDef.lastUpdateTime(get_current_timestamp_ns());
        
        // Set originator (16 chars)
        secDef.putMdEntryOriginator("UTP_SERVER");
        
        // Set symbol (16 chars)
        secDef.putSymbol(instrument.primary_symbol.c_str());
        
        secDef.securityID(static_cast<int32_t>(instrument.instrument_id));
        secDef.securityIDSource(1); // Exchange assigned
        secDef.securityType(utp_sbe::MarketDataType::Spot);
        
        // Set currencies for FX instruments
        if (auto fx_instrument = std::dynamic_pointer_cast<const market_core::FXInstrument>(
                std::const_pointer_cast<const market_core::Instrument>(
                    std::shared_ptr<market_core::Instrument>(const_cast<market_core::Instrument*>(&instrument))))) {
            // Extract currencies from FX symbol if possible
            if (fx_instrument->primary_symbol.length() >= 6) {
                secDef.putCurrency1(fx_instrument->primary_symbol.substr(0, 3).c_str());
                secDef.putCurrency2(fx_instrument->primary_symbol.substr(3, 3).c_str());
            } else {
                secDef.putCurrency1("USD");
                secDef.putCurrency2("EUR");
            }
        } else {
            secDef.putCurrency1("USD");
            secDef.putCurrency2("EUR");
        }
        
        // Set pricing parameters
        secDef.ratePrecision(5); // 5 decimal places typical for FX
        secDef.rateTerm(utp_sbe::RateTerm::Points);
        secDef.rgtsmdps(5); // Right-most digits for fractional pip
        secDef.leftDps(3);  // Left decimal places
        secDef.rightDps(5); // Right decimal places
        secDef.cls(1);      // CLS eligible
        secDef.depthOfBook(10); // 10 levels
        secDef.minTradeVol(1000); // Minimum trade volume
        
        // Update buffer size to actual encoded length
        buffer.resize(header.encodedLength() + secDef.encodedLength());
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating security definition: " << e.what() << std::endl;
        return false;
    }
}

bool UTPProtocolAdapter::create_full_refresh_from_snapshot(const market_core::SnapshotEvent& snapshot, std::vector<uint8_t>& buffer)
{
    try {
        utp_sbe::MDFullRefresh fullRefresh;
        
        // Calculate entries for bid/ask levels
        size_t num_entries = snapshot.bid_levels.size() + snapshot.ask_levels.size();
        
        // Estimate buffer size: header + fixed fields + group header + entries
        size_t estimated_size = utp_sbe::MessageHeader::encodedLength() + 
                               fullRefresh.sbeBlockLength() + 
                               4 + // Group header
                               (num_entries * 17); // Estimated entry size
        buffer.resize(estimated_size);
        
        // Encode message header
        utp_sbe::MessageHeader header;
        header.wrap(reinterpret_cast<char*>(buffer.data()), 0, header.sbeSchemaVersion(), buffer.size());
        encode_message_header(header, 20, fullRefresh.sbeBlockLength()); // Template ID 20
        
        // Wrap full refresh message
        fullRefresh.wrapForEncode(
            reinterpret_cast<char*>(buffer.data()),
            header.encodedLength(),
            buffer.size() - header.encodedLength()
        );
        
        // Set fixed fields
        fullRefresh.lastMsgSeqNumProcessed(get_next_sequence());
        fullRefresh.securityID(static_cast<int32_t>(snapshot.instrument_id));
        fullRefresh.rptSeq(static_cast<int64_t>(snapshot.sequence_number));
        fullRefresh.transactTime(get_current_timestamp_ns());
        fullRefresh.putMDEntryOriginator("UTP_SERVER");
        fullRefresh.marketDepth(static_cast<uint8_t>(std::min(snapshot.bid_levels.size(), snapshot.ask_levels.size())));
        fullRefresh.securityType(utp_sbe::MarketDataType::Spot);
        
        // Create repeating group for MD entries
        auto entries = fullRefresh.noMDEntriesCount(static_cast<uint16_t>(num_entries));
        
        // Add bid levels
        for (const auto& level : snapshot.bid_levels) {
            auto& entry = entries.next();
            entry.mdEntryType(utp_sbe::MDEntryType::BID);
            entry.mdEntryPx().mantissa(static_cast<int64_t>(level.price * 1e9)); // Convert to nanopips
            entry.mdEntrySize(static_cast<int64_t>(level.quantity));
        }
        
        // Add ask levels
        for (const auto& level : snapshot.ask_levels) {
            auto& entry = entries.next();
            entry.mdEntryType(utp_sbe::MDEntryType::OFFER);
            entry.mdEntryPx().mantissa(static_cast<int64_t>(level.price * 1e9)); // Convert to nanopips
            entry.mdEntrySize(static_cast<int64_t>(level.quantity));
        }
        
        // Update buffer size to actual encoded length
        buffer.resize(header.encodedLength() + fullRefresh.encodedLength());
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating full refresh message: " << e.what() << std::endl;
        return false;
    }
}

bool UTPProtocolAdapter::create_incremental_from_quote(const market_core::QuoteEvent& quote, std::vector<uint8_t>& buffer)
{
    try {
        utp_sbe::MDIncrementalRefresh incRefresh;
        
        // For simplicity, create one entry for this quote
        size_t num_entries = 1;
        
        // Estimate buffer size
        size_t estimated_size = utp_sbe::MessageHeader::encodedLength() + 
                               incRefresh.sbeBlockLength() + 
                               4 + // Group header
                               (num_entries * 18); // Estimated entry size
        buffer.resize(estimated_size);
        
        // Encode message header
        utp_sbe::MessageHeader header;
        header.wrap(reinterpret_cast<char*>(buffer.data()), 0, header.sbeSchemaVersion(), buffer.size());
        encode_message_header(header, 21, incRefresh.sbeBlockLength()); // Template ID 21
        
        // Wrap incremental refresh message
        incRefresh.wrapForEncode(
            reinterpret_cast<char*>(buffer.data()),
            header.encodedLength(),
            buffer.size() - header.encodedLength()
        );
        
        // Set fixed fields
        incRefresh.securityID(static_cast<int32_t>(quote.instrument_id));
        incRefresh.rptSeq(static_cast<int64_t>(quote.sequence_number));
        incRefresh.transactTime(get_current_timestamp_ns());
        incRefresh.putMDEntryOriginator("UTP_SERVER");
        
        // Create repeating group for MD entries
        auto entries = incRefresh.noMDEntriesCount(static_cast<uint16_t>(num_entries));
        
        // Add the quote update
        auto& entry = entries.next();
        
        // Map the action
        switch (quote.action) {
            case market_core::UpdateAction::ADD:
                entry.mdUpdateAction(utp_sbe::MDUpdateAction::NEW);
                break;
            case market_core::UpdateAction::CHANGE:
                entry.mdUpdateAction(utp_sbe::MDUpdateAction::CHANGE);
                break;
            case market_core::UpdateAction::DELETE:
                entry.mdUpdateAction(utp_sbe::MDUpdateAction::DELETE);
                break;
            default:
                entry.mdUpdateAction(utp_sbe::MDUpdateAction::NEW);
                break;
        }
        
        // Map the side
        entry.mdEntryType(quote.side == market_core::Side::BID ? 
            utp_sbe::MDEntryType::BID : utp_sbe::MDEntryType::OFFER);
        
        // Set price and size
        entry.mdEntryPx().mantissa(static_cast<int64_t>(quote.price * 1e9)); // Convert to nanopips
        entry.mdEntrySize(static_cast<int64_t>(quote.quantity));
        
        // Update buffer size to actual encoded length
        buffer.resize(header.encodedLength() + incRefresh.encodedLength());
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating incremental message: " << e.what() << std::endl;
        return false;
    }
}

bool UTPProtocolAdapter::send_utp_message(const std::vector<uint8_t>& message_data)
{
    if (!m_transport) {
        std::cerr << "No transport configured for UTP protocol" << std::endl;
        return false;
    }
    
    return m_transport->send_message(message_data);
}

uint64_t UTPProtocolAdapter::get_current_timestamp_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void UTPProtocolAdapter::encode_message_header(utp_sbe::MessageHeader& header, uint16_t template_id, uint16_t block_length)
{
    header.blockLength(block_length);
    header.templateId(template_id);
    header.schemaId(101);  // UTP_CLIENT_MKTDATA schema ID
    header.version(1);     // Schema version
}

void UTPProtocolAdapter::set_instrument_string(char* dest, const std::string& src, size_t max_length)
{
    std::memset(dest, 0, max_length);
    size_t copy_length = std::min(src.length(), max_length);
    std::memcpy(dest, src.c_str(), copy_length);
}

} // namespace utp_protocol