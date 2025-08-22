#include "include/cme_sbe/MDIncrementalRefreshBook46.h"
#include "include/cme_sbe/MessageHeader.h"
#include <sstream>
#include <string>

using namespace cme_sbe;

/**
 * Analyzes actual CME data to extract debug information for mock server fixes
 *
 * @param data Pointer to raw CME packet data
 * @param length Length of the packet data in bytes
 * @return String containing detailed analysis and action items for mock server
 */
std::string analyze_cme_data_for_mock_server_debug(const uint8_t* data, size_t length)
{
    std::stringstream ss;
    ss << "=== CME DATA ANALYSIS FOR MOCK SERVER DEBUG ===\n";
    ss << "Packet size: " << length << " bytes\n\n";

    if (!data || length < 12) {
        ss << "ERROR: Invalid data or packet too small for CME header\n";
        return ss.str();
    }

    try {
        size_t offset = 0;

        // Parse CME packet header (12 bytes)
        uint32_t sequence = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        uint64_t timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            timestamp |= (static_cast<uint64_t>(data[4 + i]) << (i * 8));
        }

        ss << "Packet Header:\n";
        ss << "  Sequence: " << sequence << "\n";
        ss << "  Timestamp: " << timestamp << "\n\n";

        offset = 12;
        int msg_count = 0;
        bool has_order_entries = false;
        bool proper_alignment = true;
        std::vector<uint16_t> message_sizes;
        std::vector<uint16_t> template_ids;

        // Parse each message in the packet
        while (offset + 2 <= length) {
            uint16_t msg_size = data[offset] | (data[offset + 1] << 8);
            offset += 2;

            if (msg_size == 0)
                break; // End of messages
            if (offset + msg_size > length) {
                ss << "ERROR: Message " << (msg_count + 1) << " extends beyond packet\n";
                break;
            }

            msg_count++;
            message_sizes.push_back(msg_size);

            ss << "Message " << msg_count << ":\n";
            ss << "  Size: " << msg_size << " bytes\n";

            // Check 8-byte alignment
            if (msg_size % 8 != 0) {
                proper_alignment = false;
                ss << "  ❌ NOT 8-byte aligned (remainder: " << (msg_size % 8) << ")\n";
            } else {
                ss << "  ✅ 8-byte aligned\n";
            }

            // Parse SBE message header
            if (msg_size >= 8) {
                try {
                    MessageHeader header;
                    header.wrap(const_cast<char*>(reinterpret_cast<const char*>(data)),
                        offset, 0, length);

                    uint16_t template_id = header.templateId();
                    uint16_t block_length = header.blockLength();
                    template_ids.push_back(template_id);

                    ss << "  Template ID: " << template_id;
                    if (template_id == 46)
                        ss << " (MDIncrementalRefreshBook46)";
                    ss << "\n";
                    ss << "  Block Length: " << block_length << "\n";
                    ss << "  Schema ID: " << header.schemaId() << "\n";
                    ss << "  Version: " << header.version() << "\n";

                    // Analyze content for Template 46 (MDIncrementalRefreshBook46)
                    if (template_id == 46 && block_length == 11) {
                        try {
                            MDIncrementalRefreshBook46 decoder;
                            decoder.wrapForDecode(
                                const_cast<char*>(reinterpret_cast<const char*>(data)),
                                offset + header.encodedLength(),
                                header.blockLength(),
                                header.version(),
                                length);

                            // Check MD entries
                            auto& md_entries = decoder.noMDEntries();
                            uint16_t md_count = md_entries.count();
                            ss << "  MD Entries: " << md_count << "\n";

                            // Check OrderID entries (critical for production compatibility)
                            auto& order_entries = decoder.noOrderIDEntries();
                            uint16_t order_count = order_entries.count();
                            ss << "  Order Entries: " << order_count << "\n";

                            if (order_count > 0) {
                                has_order_entries = true;
                                ss << "  ✅ HAS OrderID entries (production format)\n";

                                // Sample first few order entries
                                int sample_count = 0;
                                while (order_entries.hasNext() && sample_count < 2) {
                                    order_entries.next();
                                    sample_count++;
                                    ss << "    Order " << sample_count << ": ID=" << order_entries.orderID()
                                       << ", Priority=" << order_entries.mDOrderPriority() << "\n";
                                }
                            } else {
                                ss << "  ❌ Missing OrderID entries\n";
                            }

                            // Check transaction time
                            uint64_t transact_time = decoder.transactTime();
                            ss << "  Transaction Time: " << transact_time << "\n";

                        } catch (const std::exception& e) {
                            ss << "  Error parsing message content: " << e.what() << "\n";
                        }
                    } else if (template_id != 46) {
                        ss << "  Note: Non-incremental refresh message\n";
                    }

                } catch (const std::exception& e) {
                    ss << "  Error parsing SBE header: " << e.what() << "\n";
                }
            }

            offset += msg_size;
            ss << "\n";
        }

        // Generate mock server debug summary
        ss << "=== MOCK SERVER DEBUG SUMMARY ===\n";
        ss << "Total messages: " << msg_count << "\n";
        ss << "Message sizes: ";
        for (size_t i = 0; i < message_sizes.size(); ++i) {
            if (i > 0)
                ss << ", ";
            ss << message_sizes[i];
        }
        ss << " bytes\n";

        // Critical findings for mock server
        if (has_order_entries) {
            ss << "✅ CRITICAL: Real CME data includes OrderID entries\n";
            ss << "   → Mock server MUST implement NoOrderIDEntries group\n";
        } else {
            ss << "❌ No OrderID entries found\n";
            ss << "   → Verify this is complete production data\n";
        }

        if (proper_alignment) {
            ss << "✅ All messages are 8-byte aligned\n";
            ss << "   → Mock server alignment implementation is correct\n";
        } else {
            ss << "❌ Some messages not 8-byte aligned\n";
            ss << "   → Fix mock server: size = ((sbe_length + 7) / 8) * 8\n";
        }

        // Specific action items
        ss << "\n=== ACTION ITEMS FOR MOCK SERVER ===\n";
        ss << "1. POPULATE OrderID entries:\n";
        ss << "   - Add realistic order IDs (e.g., incremental numbers)\n";
        ss << "   - Set proper MDOrderPriority values\n";
        ss << "   - Include MDDisplayQty for each order\n";

        ss << "2. VERIFY message sizes match exactly:\n";
        for (size_t i = 0; i < message_sizes.size(); ++i) {
            ss << "   - Message " << (i + 1) << ": " << message_sizes[i] << " bytes\n";
        }

        ss << "3. ENSURE 8-byte alignment:\n";
        ss << "   - Use formula: ((sbe_length + 7) / 8) * 8\n";
        ss << "   - All messages must be multiples of 8 bytes\n";

        ss << "4. VALIDATE SBE structure:\n";
        ss << "   - Template ID 46 (MDIncrementalRefreshBook46)\n";
        ss << "   - Block Length 11\n";
        ss << "   - Schema ID 1, Version 13\n";

        if (has_order_entries) {
            ss << "\n🔥 PRIORITY: Mock server is missing OrderID entries!\n";
            ss << "This is why real CME clients crash - they expect order-level data.\n";
        }

    } catch (const std::exception& e) {
        ss << "Analysis error: " << e.what() << "\n";
    }

    return ss.str();
}