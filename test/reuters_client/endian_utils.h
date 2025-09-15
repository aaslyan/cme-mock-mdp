#pragma once

#include <cstdint>
#include <cstring>

// Native endian conversion utilities to replace Boost dependency

namespace endian_utils {

// Check system endianness at compile time
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
constexpr bool is_big_endian = true;
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
constexpr bool is_big_endian = false;
#else
// Runtime detection fallback
inline bool detect_endianness()
{
    uint16_t test = 0x0102;
    return *reinterpret_cast<uint8_t*>(&test) == 0x01;
}
const bool is_big_endian = detect_endianness();
#endif

// Byte swap functions
inline uint16_t swap_bytes_16(uint16_t value)
{
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

inline uint32_t swap_bytes_32(uint32_t value)
{
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) | ((value & 0x000000FF) << 24);
}

inline uint64_t swap_bytes_64(uint64_t value)
{
    return ((value & 0xFF00000000000000ULL) >> 56) | ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0x0000FF0000000000ULL) >> 24) | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x00000000FF000000ULL) << 8) | ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x000000000000FF00ULL) << 40) | ((value & 0x00000000000000FFULL) << 56);
}

// Big endian to native conversions
template <typename T>
inline T big_to_native(T value);

template <>
inline uint16_t big_to_native<uint16_t>(uint16_t value)
{
    if (is_big_endian) {
        return value;
    } else {
        return swap_bytes_16(value);
    }
}

template <>
inline uint32_t big_to_native<uint32_t>(uint32_t value)
{
    if (is_big_endian) {
        return value;
    } else {
        return swap_bytes_32(value);
    }
}

template <>
inline uint64_t big_to_native<uint64_t>(uint64_t value)
{
    if (is_big_endian) {
        return value;
    } else {
        return swap_bytes_64(value);
    }
}

template <>
inline int16_t big_to_native<int16_t>(int16_t value)
{
    return static_cast<int16_t>(big_to_native(static_cast<uint16_t>(value)));
}

template <>
inline int32_t big_to_native<int32_t>(int32_t value)
{
    return static_cast<int32_t>(big_to_native(static_cast<uint32_t>(value)));
}

template <>
inline int64_t big_to_native<int64_t>(int64_t value)
{
    return static_cast<int64_t>(big_to_native(static_cast<uint64_t>(value)));
}

// Native to big endian conversions
template <typename T>
inline T native_to_big(T value);

template <>
inline uint16_t native_to_big<uint16_t>(uint16_t value)
{
    if (is_big_endian) {
        return value;
    } else {
        return swap_bytes_16(value);
    }
}

template <>
inline uint32_t native_to_big<uint32_t>(uint32_t value)
{
    if (is_big_endian) {
        return value;
    } else {
        return swap_bytes_32(value);
    }
}

template <>
inline uint64_t native_to_big<uint64_t>(uint64_t value)
{
    if (is_big_endian) {
        return value;
    } else {
        return swap_bytes_64(value);
    }
}

template <>
inline int16_t native_to_big<int16_t>(int16_t value)
{
    return static_cast<int16_t>(native_to_big(static_cast<uint16_t>(value)));
}

template <>
inline int32_t native_to_big<int32_t>(int32_t value)
{
    return static_cast<int32_t>(native_to_big(static_cast<uint32_t>(value)));
}

template <>
inline int64_t native_to_big<int64_t>(int64_t value)
{
    return static_cast<int64_t>(native_to_big(static_cast<uint64_t>(value)));
}

// Little endian to native conversions
template <typename T>
inline T little_to_native(T value);

template <>
inline uint16_t little_to_native<uint16_t>(uint16_t value)
{
    if (!is_big_endian) {
        return value;
    } else {
        return swap_bytes_16(value);
    }
}

template <>
inline uint32_t little_to_native<uint32_t>(uint32_t value)
{
    if (!is_big_endian) {
        return value;
    } else {
        return swap_bytes_32(value);
    }
}

template <>
inline uint64_t little_to_native<uint64_t>(uint64_t value)
{
    if (!is_big_endian) {
        return value;
    } else {
        return swap_bytes_64(value);
    }
}

// Native to little endian conversions
template <typename T>
inline T native_to_little(T value);

template <>
inline uint16_t native_to_little<uint16_t>(uint16_t value)
{
    if (!is_big_endian) {
        return value;
    } else {
        return swap_bytes_16(value);
    }
}

template <>
inline uint32_t native_to_little<uint32_t>(uint32_t value)
{
    if (!is_big_endian) {
        return value;
    } else {
        return swap_bytes_32(value);
    }
}

template <>
inline uint64_t native_to_little<uint64_t>(uint64_t value)
{
    if (!is_big_endian) {
        return value;
    } else {
        return swap_bytes_64(value);
    }
}

} // namespace endian_utils