#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cme_mock {

/**
 * Print hexdump of binary data in standard format with ASCII representation
 * @param data Pointer to binary data
 * @param length Number of bytes to dump
 * @param label Optional label to print before the dump
 */
void hexdump(const uint8_t* data, size_t length, const std::string& label = "");

/**
 * Return hexdump of binary data as a string
 * @param data Pointer to binary data
 * @param length Number of bytes to dump
 * @return Formatted hexdump string
 */
std::string hexdump_string(const uint8_t* data, size_t length);

/**
 * Print compact hexdump (hex bytes only, single line)
 * @param data Pointer to binary data
 * @param length Number of bytes to dump
 * @param label Optional label to print before the dump
 */
void hexdump_compact(const uint8_t* data, size_t length, const std::string& label = "");

// Convenience overloads for std::vector<uint8_t>
inline void hexdump(const std::vector<uint8_t>& data, const std::string& label = "")
{
    hexdump(data.data(), data.size(), label);
}

inline std::string hexdump_string(const std::vector<uint8_t>& data)
{
    return hexdump_string(data.data(), data.size());
}

inline void hexdump_compact(const std::vector<uint8_t>& data, const std::string& label = "")
{
    hexdump_compact(data.data(), data.size(), label);
}

} // namespace cme_mock