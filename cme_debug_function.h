#ifndef CME_DEBUG_FUNCTION_H
#define CME_DEBUG_FUNCTION_H

#include <string>
#include <cstdint>
#include <cstddef>

/**
 * Analyzes actual CME data to extract debug information for mock server fixes
 * 
 * This function takes raw CME packet data and analyzes it to identify what 
 * needs to be fixed in a mock server implementation. It checks for:
 * - Message structure and alignment
 * - SBE header validation
 * - OrderID entries presence (critical for production compatibility)
 * - Message sizes and template IDs
 * 
 * @param data Pointer to raw CME packet data
 * @param length Length of the packet data in bytes
 * @return String containing detailed analysis and action items for mock server
 * 
 * Usage example:
 * ```cpp
 * const uint8_t* cme_packet = get_real_cme_data();
 * size_t packet_size = get_packet_size();
 * std::string debug_info = analyze_cme_data_for_mock_server_debug(cme_packet, packet_size);
 * std::cout << debug_info << std::endl;
 * ```
 */
std::string analyze_cme_data_for_mock_server_debug(const uint8_t* data, size_t length);

#endif // CME_DEBUG_FUNCTION_H