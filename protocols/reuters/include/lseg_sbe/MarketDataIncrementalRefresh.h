/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_MARKETDATAINCREMENTALREFRESH_CXX_H_
#define _LSEG_SBE_MARKETDATAINCREMENTALREFRESH_CXX_H_

#if __cplusplus >= 201103L
#  define SBE_CONSTEXPR constexpr
#  define SBE_NOEXCEPT noexcept
#else
#  define SBE_CONSTEXPR
#  define SBE_NOEXCEPT
#endif

#if __cplusplus >= 201703L
#  include <string_view>
#  define SBE_NODISCARD [[nodiscard]]
#  if !defined(SBE_USE_STRING_VIEW)
#    define SBE_USE_STRING_VIEW 1
#  endif
#else
#  define SBE_NODISCARD
#endif

#if __cplusplus >= 202002L
#  include <span>
#  if !defined(SBE_USE_SPAN)
#    define SBE_USE_SPAN 1
#  endif
#endif

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <limits>
#include <cstring>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

#if defined(WIN32) || defined(_WIN32)
#  define SBE_BIG_ENDIAN_ENCODE_16(v) _byteswap_ushort(v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) _byteswap_ulong(v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) _byteswap_uint64(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) (v)
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define SBE_BIG_ENDIAN_ENCODE_16(v) __builtin_bswap16(v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) __builtin_bswap32(v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) __builtin_bswap64(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) (v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) (v)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define SBE_LITTLE_ENDIAN_ENCODE_16(v) __builtin_bswap16(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_32(v) __builtin_bswap32(v)
#  define SBE_LITTLE_ENDIAN_ENCODE_64(v) __builtin_bswap64(v)
#  define SBE_BIG_ENDIAN_ENCODE_16(v) (v)
#  define SBE_BIG_ENDIAN_ENCODE_32(v) (v)
#  define SBE_BIG_ENDIAN_ENCODE_64(v) (v)
#else
#  error "Byte Ordering of platform not determined. Set __BYTE_ORDER__ manually before including this file."
#endif

#if !defined(SBE_BOUNDS_CHECK_EXPECT)
#  if defined(SBE_NO_BOUNDS_CHECK)
#    define SBE_BOUNDS_CHECK_EXPECT(exp, c) (false)
#  elif defined(_MSC_VER)
#    define SBE_BOUNDS_CHECK_EXPECT(exp, c) (exp)
#  else 
#    define SBE_BOUNDS_CHECK_EXPECT(exp, c) (__builtin_expect(exp, c))
#  endif

#endif

#define SBE_FLOAT_NAN std::numeric_limits<float>::quiet_NaN()
#define SBE_DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()
#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()


#include "LegIDEnum.h"
#include "TimestampEncoding.h"
#include "VarString.h"
#include "QtyEncoding.h"
#include "Credentials.h"
#include "FlowTypeEnum.h"
#include "SubscriptionRequestTypeEnumSecDef.h"
#include "GroupSizeEncoding.h"
#include "SubscriptionRequestTypeEnum.h"
#include "MarketSegmentIDEnum.h"
#include "MessageTypeEnum.h"
#include "SecurityTradingStatusEnum.h"
#include "BooleanEnum.h"
#include "PartyRoleEnum.h"
#include "EntryTypeEnum.h"
#include "UpdateActionEnum.h"
#include "SessionRejectCodeEnum.h"
#include "SecurityStatusEnum.h"
#include "SecurityResponseTypeEnum.h"
#include "ExDestinationEnum.h"
#include "EncryptionTypeEnum.h"
#include "SecurityAltIDSourceEnum.h"
#include "MessageHeader.h"
#include "TerminationCodeEnum.h"
#include "AggressorSideEnum.h"
#include "ScreenedMDRequestTypeEnum.h"
#include "LegSecurityIDSourceEnum.h"
#include "HaltReasonEnum.h"
#include "MessageEventSourceEnum.h"
#include "Classification.h"
#include "DecimalEncoding.h"
#include "ThresholdEncoding.h"
#include "PartyIDSourceEnum.h"
#include "DATA.h"
#include "SecurityRejectReasonEnum.h"
#include "SecurityRequestTypeEnum.h"

namespace lseg_sbe {

class MarketDataIncrementalRefresh
{
private:
    char *m_buffer = nullptr;
    std::uint64_t m_bufferLength = 0;
    std::uint64_t m_offset = 0;
    std::uint64_t m_position = 0;
    std::uint64_t m_actingBlockLength = 0;
    std::uint64_t m_actingVersion = 0;

    inline std::uint64_t *sbePositionPtr() SBE_NOEXCEPT
    {
        return &m_position;
    }

public:
    static constexpr std::uint16_t SBE_BLOCK_LENGTH = static_cast<std::uint16_t>(33);
    static constexpr std::uint16_t SBE_TEMPLATE_ID = static_cast<std::uint16_t>(9);
    static constexpr std::uint16_t SBE_SCHEMA_ID = static_cast<std::uint16_t>(101);
    static constexpr std::uint16_t SBE_SCHEMA_VERSION = static_cast<std::uint16_t>(52);
    static constexpr const char* SBE_SEMANTIC_VERSION = "";

    enum MetaAttribute
    {
        EPOCH, TIME_UNIT, SEMANTIC_TYPE, PRESENCE
    };

    union sbe_float_as_uint_u
    {
        float fp_value;
        std::uint32_t uint_value;
    };

    union sbe_double_as_uint_u
    {
        double fp_value;
        std::uint64_t uint_value;
    };

    using messageHeader = MessageHeader;

    MarketDataIncrementalRefresh() = default;

    MarketDataIncrementalRefresh(
        char *buffer,
        const std::uint64_t offset,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        m_buffer(buffer),
        m_bufferLength(bufferLength),
        m_offset(offset),
        m_position(sbeCheckPosition(offset + actingBlockLength)),
        m_actingBlockLength(actingBlockLength),
        m_actingVersion(actingVersion)
    {
    }

    MarketDataIncrementalRefresh(char *buffer, const std::uint64_t bufferLength) :
        MarketDataIncrementalRefresh(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion())
    {
    }

    MarketDataIncrementalRefresh(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        MarketDataIncrementalRefresh(buffer, 0, bufferLength, actingBlockLength, actingVersion)
    {
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeBlockLength() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(33);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sbeBlockAndHeaderLength() SBE_NOEXCEPT
    {
        return messageHeader::encodedLength() + sbeBlockLength();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeTemplateId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(9);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(101);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeSchemaVersion() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(52);
    }

    SBE_NODISCARD static const char *sbeSemanticVersion() SBE_NOEXCEPT
    {
        return "";
    }

    SBE_NODISCARD static SBE_CONSTEXPR const char *sbeSemanticType() SBE_NOEXCEPT
    {
        return "X";
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    MarketDataIncrementalRefresh &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingBlockLength = sbeBlockLength();
        m_actingVersion = sbeSchemaVersion();
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    MarketDataIncrementalRefresh &wrapAndApplyHeader(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        messageHeader hdr(buffer, offset, bufferLength, sbeSchemaVersion());

        hdr
            .blockLength(sbeBlockLength())
            .templateId(sbeTemplateId())
            .schemaId(sbeSchemaId())
            .version(sbeSchemaVersion());

        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset + messageHeader::encodedLength();
        m_actingBlockLength = sbeBlockLength();
        m_actingVersion = sbeSchemaVersion();
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    MarketDataIncrementalRefresh &wrapForDecode(
        char *buffer,
        const std::uint64_t offset,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion,
        const std::uint64_t bufferLength)
    {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingBlockLength = actingBlockLength;
        m_actingVersion = actingVersion;
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    MarketDataIncrementalRefresh &sbeRewind()
    {
        return wrapForDecode(m_buffer, m_offset, m_actingBlockLength, m_actingVersion, m_bufferLength);
    }

    SBE_NODISCARD std::uint64_t sbePosition() const SBE_NOEXCEPT
    {
        return m_position;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    std::uint64_t sbeCheckPosition(const std::uint64_t position)
    {
        if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
        {
            throw std::runtime_error("buffer too short [E100]");
        }
        return position;
    }

    void sbePosition(const std::uint64_t position)
    {
        m_position = sbeCheckPosition(position);
    }

    SBE_NODISCARD std::uint64_t encodedLength() const SBE_NOEXCEPT
    {
        return sbePosition() - m_offset;
    }

    SBE_NODISCARD std::uint64_t decodeLength() const
    {
        MarketDataIncrementalRefresh skipper(m_buffer, m_offset, m_bufferLength, m_actingBlockLength, m_actingVersion);
        skipper.skip();
        return skipper.encodedLength();
    }

    SBE_NODISCARD const char *buffer() const SBE_NOEXCEPT
    {
        return m_buffer;
    }

    SBE_NODISCARD char *buffer() SBE_NOEXCEPT
    {
        return m_buffer;
    }

    SBE_NODISCARD std::uint64_t bufferLength() const SBE_NOEXCEPT
    {
        return m_bufferLength;
    }

    SBE_NODISCARD std::uint64_t actingVersion() const SBE_NOEXCEPT
    {
        return m_actingVersion;
    }

    SBE_NODISCARD static const char *ApplIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t applIDId() SBE_NOEXCEPT
    {
        return 1180;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t applIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool applIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t applIDEncodingOffset() SBE_NOEXCEPT
    {
        return 0;
    }

    static SBE_CONSTEXPR char applIDNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char applIDMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char applIDMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t applIDEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t applIDLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *applID() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char *applID() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char applID(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for applID [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
        return (val);
    }

    MarketDataIncrementalRefresh &applID(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for applID [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getApplID(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getApplID [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getApplIDAsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 8);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    MarketDataIncrementalRefresh &putApplID(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 8, "array too large for putApplID");

        std::memcpy(m_buffer + m_offset + 0, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 8; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    MarketDataIncrementalRefresh &putApplID(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    MarketDataIncrementalRefresh &putApplID(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 8);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    MarketDataIncrementalRefresh &putApplID(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putApplID(std::span<const char, N>(src));
    }
    #endif

    SBE_NODISCARD std::string getApplIDAsString() const
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getApplIDAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getApplIDAsString();

        for (const auto c : s)
        {
            switch (c)
            {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;

                default:
                    if ('\x00' <= c && c <= '\x1f')
                    {
                        oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << (int)(c);
                    }
                    else
                    {
                        oss << c;
                    }
            }
        }

        return oss.str();
    }

    #ifdef SBE_USE_STRING_VIEW
    SBE_NODISCARD std::string_view getApplIDAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    MarketDataIncrementalRefresh &putApplID(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putApplID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #else
    MarketDataIncrementalRefresh &putApplID(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putApplID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *BookStateIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "SeqNum";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t bookStateIDId() SBE_NOEXCEPT
    {
        return 1181;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t bookStateIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool bookStateIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t bookStateIDEncodingOffset() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t bookStateIDNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t bookStateIDMinValue() SBE_NOEXCEPT
    {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t bookStateIDMaxValue() SBE_NOEXCEPT
    {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t bookStateIDEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD std::uint64_t bookStateID() const SBE_NOEXCEPT
    {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 8, sizeof(std::uint64_t));
        return SBE_BIG_ENDIAN_ENCODE_64(val);
    }

    MarketDataIncrementalRefresh &bookStateID(const std::uint64_t value) SBE_NOEXCEPT
    {
        std::uint64_t val = SBE_BIG_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 8, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *ConflationIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t conflationIDId() SBE_NOEXCEPT
    {
        return 30782;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t conflationIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool conflationIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t conflationIDEncodingOffset() SBE_NOEXCEPT
    {
        return 16;
    }

    static SBE_CONSTEXPR std::uint64_t conflationIDNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t conflationIDMinValue() SBE_NOEXCEPT
    {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t conflationIDMaxValue() SBE_NOEXCEPT
    {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t conflationIDEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD std::uint64_t conflationID() const SBE_NOEXCEPT
    {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 16, sizeof(std::uint64_t));
        return SBE_BIG_ENDIAN_ENCODE_64(val);
    }

    MarketDataIncrementalRefresh &conflationID(const std::uint64_t value) SBE_NOEXCEPT
    {
        std::uint64_t val = SBE_BIG_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 16, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *CreditStateIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t creditStateIDId() SBE_NOEXCEPT
    {
        return 30785;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t creditStateIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool creditStateIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t creditStateIDEncodingOffset() SBE_NOEXCEPT
    {
        return 24;
    }

    static SBE_CONSTEXPR std::uint64_t creditStateIDNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t creditStateIDMinValue() SBE_NOEXCEPT
    {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t creditStateIDMaxValue() SBE_NOEXCEPT
    {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t creditStateIDEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD std::uint64_t creditStateID() const SBE_NOEXCEPT
    {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 24, sizeof(std::uint64_t));
        return SBE_BIG_ENDIAN_ENCODE_64(val);
    }

    MarketDataIncrementalRefresh &creditStateID(const std::uint64_t value) SBE_NOEXCEPT
    {
        std::uint64_t val = SBE_BIG_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 24, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *MessageEventSourceMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t messageEventSourceId() SBE_NOEXCEPT
    {
        return 30804;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t messageEventSourceSinceVersion() SBE_NOEXCEPT
    {
        return 46;
    }

    SBE_NODISCARD bool messageEventSourceInActingVersion() SBE_NOEXCEPT
    {
        return m_actingVersion >= messageEventSourceSinceVersion();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageEventSourceEncodingOffset() SBE_NOEXCEPT
    {
        return 32;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageEventSourceEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t messageEventSourceRaw() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 46)
        {
            return static_cast<std::uint8_t>(255);
        }

        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 32, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD MessageEventSourceEnum::Value messageEventSource() const
    {
        if (m_actingVersion < 46)
        {
            return MessageEventSourceEnum::NULL_VALUE;
        }

        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 32, sizeof(std::uint8_t));
        return MessageEventSourceEnum::get((val));
    }

    MarketDataIncrementalRefresh &messageEventSource(const MessageEventSourceEnum::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 32, &val, sizeof(std::uint8_t));
        return *this;
    }

    class MDIncGrp
    {
    private:
        char *m_buffer = nullptr;
        std::uint64_t m_bufferLength = 0;
        std::uint64_t m_initialPosition = 0;
        std::uint64_t *m_positionPtr = nullptr;
        std::uint64_t m_blockLength = 0;
        std::uint64_t m_count = 0;
        std::uint64_t m_index = 0;
        std::uint64_t m_offset = 0;
        std::uint64_t m_actingVersion = 0;

        SBE_NODISCARD std::uint64_t *sbePositionPtr() SBE_NOEXCEPT
        {
            return m_positionPtr;
        }

    public:
        MDIncGrp() = default;

        inline void wrapForDecode(
            char *buffer,
            std::uint64_t *pos,
            const std::uint64_t actingVersion,
            const std::uint64_t bufferLength)
        {
            GroupSizeEncoding dimensions(buffer, *pos, bufferLength, actingVersion);
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            m_blockLength = dimensions.blockLength();
            m_count = dimensions.numInGroup();
            m_index = 0;
            m_actingVersion = actingVersion;
            m_initialPosition = *pos;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 8;
        }

        inline void wrapForEncode(
            char *buffer,
            const std::uint16_t count,
            std::uint64_t *pos,
            const std::uint64_t actingVersion,
            const std::uint64_t bufferLength)
        {
    #if defined(__GNUG__) && !defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtype-limits"
    #endif
            if (count > 65534)
            {
                throw std::runtime_error("count outside of allowed range [E110]");
            }
    #if defined(__GNUG__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
            m_buffer = buffer;
            m_bufferLength = bufferLength;
            GroupSizeEncoding dimensions(buffer, *pos, bufferLength, actingVersion);
            dimensions.blockLength(static_cast<std::uint16_t>(88));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 88;
            m_actingVersion = actingVersion;
            m_initialPosition = *pos;
            m_positionPtr = pos;
            *m_positionPtr = *m_positionPtr + 8;
        }

        static SBE_CONSTEXPR std::uint64_t sbeHeaderSize() SBE_NOEXCEPT
        {
            return 8;
        }

        static SBE_CONSTEXPR std::uint64_t sbeBlockLength() SBE_NOEXCEPT
        {
            return 88;
        }

        SBE_NODISCARD std::uint64_t sbeActingBlockLength() SBE_NOEXCEPT
        {
            return m_blockLength;
        }

        SBE_NODISCARD std::uint64_t sbePosition() const SBE_NOEXCEPT
        {
            return *m_positionPtr;
        }

        // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
        std::uint64_t sbeCheckPosition(const std::uint64_t position)
        {
            if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
            {
                throw std::runtime_error("buffer too short [E100]");
            }
            return position;
        }

        void sbePosition(const std::uint64_t position)
        {
            *m_positionPtr = sbeCheckPosition(position);
        }

        SBE_NODISCARD inline std::uint64_t count() const SBE_NOEXCEPT
        {
            return m_count;
        }

        SBE_NODISCARD inline bool hasNext() const SBE_NOEXCEPT
        {
            return m_index < m_count;
        }

        inline MDIncGrp &next()
        {
            if (m_index >= m_count)
            {
                throw std::runtime_error("index >= count [E108]");
            }
            m_offset = *m_positionPtr;
            if (SBE_BOUNDS_CHECK_EXPECT(((m_offset + m_blockLength) > m_bufferLength), false))
            {
                throw std::runtime_error("buffer too short for next group index [E108]");
            }
            *m_positionPtr = m_offset + m_blockLength;
            ++m_index;

            return *this;
        }

        inline std::uint64_t resetCountToIndex()
        {
            m_count = m_index;
            GroupSizeEncoding dimensions(m_buffer, m_initialPosition, m_bufferLength, m_actingVersion);
            dimensions.numInGroup(static_cast<std::uint16_t>(m_count));
            return m_count;
        }

        template<class Func> inline void forEach(Func &&func)
        {
            while (hasNext())
            {
                next();
                func(*this);
            }
        }


        SBE_NODISCARD static const char *MDUpdateActionMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "char";
                case MetaAttribute::PRESENCE: return "required";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDUpdateActionId() SBE_NOEXCEPT
        {
            return 279;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDUpdateActionSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDUpdateActionInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDUpdateActionEncodingOffset() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDUpdateActionEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD char mDUpdateActionRaw() const SBE_NOEXCEPT
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 0, sizeof(char));
            return (val);
        }

        SBE_NODISCARD UpdateActionEnum::Value mDUpdateAction() const
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 0, sizeof(char));
            return UpdateActionEnum::get((val));
        }

        MDIncGrp &mDUpdateAction(const UpdateActionEnum::Value value) SBE_NOEXCEPT
        {
            char val = (value);
            std::memcpy(m_buffer + m_offset + 0, &val, sizeof(char));
            return *this;
        }

        SBE_NODISCARD static const char *MDSubBookTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDSubBookTypeId() SBE_NOEXCEPT
        {
            return 1173;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDSubBookTypeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDSubBookTypeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDSubBookTypeEncodingOffset() SBE_NOEXCEPT
        {
            return 1;
        }

        static SBE_CONSTEXPR std::uint8_t mDSubBookTypeNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_UINT8;
        }

        static SBE_CONSTEXPR std::uint8_t mDSubBookTypeMinValue() SBE_NOEXCEPT
        {
            return static_cast<std::uint8_t>(0);
        }

        static SBE_CONSTEXPR std::uint8_t mDSubBookTypeMaxValue() SBE_NOEXCEPT
        {
            return static_cast<std::uint8_t>(254);
        }

        static SBE_CONSTEXPR std::size_t mDSubBookTypeEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD std::uint8_t mDSubBookType() const SBE_NOEXCEPT
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 1, sizeof(std::uint8_t));
            return (val);
        }

        MDIncGrp &mDSubBookType(const std::uint8_t value) SBE_NOEXCEPT
        {
            std::uint8_t val = (value);
            std::memcpy(m_buffer + m_offset + 1, &val, sizeof(std::uint8_t));
            return *this;
        }

        SBE_NODISCARD static const char *MDEntryTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "char";
                case MetaAttribute::PRESENCE: return "required";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryTypeId() SBE_NOEXCEPT
        {
            return 269;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryTypeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryTypeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryTypeEncodingOffset() SBE_NOEXCEPT
        {
            return 2;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryTypeEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD char mDEntryTypeRaw() const SBE_NOEXCEPT
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 2, sizeof(char));
            return (val);
        }

        SBE_NODISCARD EntryTypeEnum::Value mDEntryType() const
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 2, sizeof(char));
            return EntryTypeEnum::get((val));
        }

        MDIncGrp &mDEntryType(const EntryTypeEnum::Value value) SBE_NOEXCEPT
        {
            char val = (value);
            std::memcpy(m_buffer + m_offset + 2, &val, sizeof(char));
            return *this;
        }

        SBE_NODISCARD static const char *MDEntryIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryIDId() SBE_NOEXCEPT
        {
            return 278;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryIDSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryIDInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryIDEncodingOffset() SBE_NOEXCEPT
        {
            return 3;
        }

        static SBE_CONSTEXPR char mDEntryIDNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char mDEntryIDMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char mDEntryIDMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t mDEntryIDEncodingLength() SBE_NOEXCEPT
        {
            return 20;
        }

        static SBE_CONSTEXPR std::uint64_t mDEntryIDLength() SBE_NOEXCEPT
        {
            return 20;
        }

        SBE_NODISCARD const char *mDEntryID() const SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 3;
        }

        SBE_NODISCARD char *mDEntryID() SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 3;
        }

        SBE_NODISCARD char mDEntryID(const std::uint64_t index) const
        {
            if (index >= 20)
            {
                throw std::runtime_error("index out of range for mDEntryID [E104]");
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 3 + (index * 1), sizeof(char));
            return (val);
        }

        MDIncGrp &mDEntryID(const std::uint64_t index, const char value)
        {
            if (index >= 20)
            {
                throw std::runtime_error("index out of range for mDEntryID [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 3 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getMDEntryID(char *const dst, const std::uint64_t length) const
        {
            if (length > 20)
            {
                throw std::runtime_error("length too large for getMDEntryID [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 3, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getMDEntryIDAsSpan() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 3;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 20);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        MDIncGrp &putMDEntryID(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 20, "array too large for putMDEntryID");

            std::memcpy(m_buffer + m_offset + 3, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 20; ++start)
            {
                m_buffer[m_offset + 3 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        MDIncGrp &putMDEntryID(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        MDIncGrp &putMDEntryID(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 3, src, sizeof(char) * 20);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        MDIncGrp &putMDEntryID(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putMDEntryID(std::span<const char, N>(src));
        }
        #endif

        SBE_NODISCARD std::string getMDEntryIDAsString() const
        {
            const char *buffer = m_buffer + m_offset + 3;
            std::size_t length = 0;

            for (; length < 20 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getMDEntryIDAsJsonEscapedString()
        {
            std::ostringstream oss;
            std::string s = getMDEntryIDAsString();

            for (const auto c : s)
            {
                switch (c)
                {
                    case '"': oss << "\\\""; break;
                    case '\\': oss << "\\\\"; break;
                    case '\b': oss << "\\b"; break;
                    case '\f': oss << "\\f"; break;
                    case '\n': oss << "\\n"; break;
                    case '\r': oss << "\\r"; break;
                    case '\t': oss << "\\t"; break;

                    default:
                        if ('\x00' <= c && c <= '\x1f')
                        {
                            oss << "\\u" << std::hex << std::setw(4)
                                << std::setfill('0') << (int)(c);
                        }
                        else
                        {
                            oss << c;
                        }
                }
            }

            return oss.str();
        }

        #ifdef SBE_USE_STRING_VIEW
        SBE_NODISCARD std::string_view getMDEntryIDAsStringView() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 3;
            std::size_t length = 0;

            for (; length < 20 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        MDIncGrp &putMDEntryID(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 20)
            {
                throw std::runtime_error("string too large for putMDEntryID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 3, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 20; ++start)
            {
                m_buffer[m_offset + 3 + start] = 0;
            }

            return *this;
        }
        #else
        MDIncGrp &putMDEntryID(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 20)
            {
                throw std::runtime_error("string too large for putMDEntryID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 3, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 20; ++start)
            {
                m_buffer[m_offset + 3 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *SecurityIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "required";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t securityIDId() SBE_NOEXCEPT
        {
            return 48;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityIDSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool securityIDInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityIDEncodingOffset() SBE_NOEXCEPT
        {
            return 23;
        }

        static SBE_CONSTEXPR char securityIDNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char securityIDMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char securityIDMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t securityIDEncodingLength() SBE_NOEXCEPT
        {
            return 10;
        }

        static SBE_CONSTEXPR std::uint64_t securityIDLength() SBE_NOEXCEPT
        {
            return 10;
        }

        SBE_NODISCARD const char *securityID() const SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 23;
        }

        SBE_NODISCARD char *securityID() SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 23;
        }

        SBE_NODISCARD char securityID(const std::uint64_t index) const
        {
            if (index >= 10)
            {
                throw std::runtime_error("index out of range for securityID [E104]");
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 23 + (index * 1), sizeof(char));
            return (val);
        }

        MDIncGrp &securityID(const std::uint64_t index, const char value)
        {
            if (index >= 10)
            {
                throw std::runtime_error("index out of range for securityID [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 23 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getSecurityID(char *const dst, const std::uint64_t length) const
        {
            if (length > 10)
            {
                throw std::runtime_error("length too large for getSecurityID [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 23, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getSecurityIDAsSpan() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 23;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 10);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        MDIncGrp &putSecurityID(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 10, "array too large for putSecurityID");

            std::memcpy(m_buffer + m_offset + 23, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 10; ++start)
            {
                m_buffer[m_offset + 23 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        MDIncGrp &putSecurityID(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        MDIncGrp &putSecurityID(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 23, src, sizeof(char) * 10);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        MDIncGrp &putSecurityID(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putSecurityID(std::span<const char, N>(src));
        }
        #endif

        SBE_NODISCARD std::string getSecurityIDAsString() const
        {
            const char *buffer = m_buffer + m_offset + 23;
            std::size_t length = 0;

            for (; length < 10 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getSecurityIDAsJsonEscapedString()
        {
            std::ostringstream oss;
            std::string s = getSecurityIDAsString();

            for (const auto c : s)
            {
                switch (c)
                {
                    case '"': oss << "\\\""; break;
                    case '\\': oss << "\\\\"; break;
                    case '\b': oss << "\\b"; break;
                    case '\f': oss << "\\f"; break;
                    case '\n': oss << "\\n"; break;
                    case '\r': oss << "\\r"; break;
                    case '\t': oss << "\\t"; break;

                    default:
                        if ('\x00' <= c && c <= '\x1f')
                        {
                            oss << "\\u" << std::hex << std::setw(4)
                                << std::setfill('0') << (int)(c);
                        }
                        else
                        {
                            oss << c;
                        }
                }
            }

            return oss.str();
        }

        #ifdef SBE_USE_STRING_VIEW
        SBE_NODISCARD std::string_view getSecurityIDAsStringView() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 23;
            std::size_t length = 0;

            for (; length < 10 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        MDIncGrp &putSecurityID(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 10)
            {
                throw std::runtime_error("string too large for putSecurityID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 23, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 10; ++start)
            {
                m_buffer[m_offset + 23 + start] = 0;
            }

            return *this;
        }
        #else
        MDIncGrp &putSecurityID(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 10)
            {
                throw std::runtime_error("string too large for putSecurityID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 23, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 10; ++start)
            {
                m_buffer[m_offset + 23 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *MDEntryPxMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryPxId() SBE_NOEXCEPT
        {
            return 270;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryPxSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryPxInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryPxEncodingOffset() SBE_NOEXCEPT
        {
            return 33;
        }

private:
        DecimalEncoding m_mDEntryPx;

public:
        SBE_NODISCARD DecimalEncoding &mDEntryPx()
        {
            m_mDEntryPx.wrap(m_buffer, m_offset + 33, m_actingVersion, m_bufferLength);
            return m_mDEntryPx;
        }

        SBE_NODISCARD static const char *MDEntrySizeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "Qty";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntrySizeId() SBE_NOEXCEPT
        {
            return 271;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntrySizeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntrySizeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntrySizeEncodingOffset() SBE_NOEXCEPT
        {
            return 41;
        }

private:
        QtyEncoding m_mDEntrySize;

public:
        SBE_NODISCARD QtyEncoding &mDEntrySize()
        {
            m_mDEntrySize.wrap(m_buffer, m_offset + 41, m_actingVersion, m_bufferLength);
            return m_mDEntrySize;
        }

        SBE_NODISCARD static const char *MDEntryTimeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "UTCTimeOnly";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryTimeId() SBE_NOEXCEPT
        {
            return 273;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryTimeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryTimeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryTimeEncodingOffset() SBE_NOEXCEPT
        {
            return 49;
        }

private:
        TimestampEncoding m_mDEntryTime;

public:
        SBE_NODISCARD TimestampEncoding &mDEntryTime()
        {
            m_mDEntryTime.wrap(m_buffer, m_offset + 49, m_actingVersion, m_bufferLength);
            return m_mDEntryTime;
        }

        SBE_NODISCARD static const char *NumberOfOrdersMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t numberOfOrdersId() SBE_NOEXCEPT
        {
            return 346;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t numberOfOrdersSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool numberOfOrdersInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t numberOfOrdersEncodingOffset() SBE_NOEXCEPT
        {
            return 57;
        }

        static SBE_CONSTEXPR std::uint32_t numberOfOrdersNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_UINT32;
        }

        static SBE_CONSTEXPR std::uint32_t numberOfOrdersMinValue() SBE_NOEXCEPT
        {
            return UINT32_C(0x0);
        }

        static SBE_CONSTEXPR std::uint32_t numberOfOrdersMaxValue() SBE_NOEXCEPT
        {
            return UINT32_C(0xfffffffe);
        }

        static SBE_CONSTEXPR std::size_t numberOfOrdersEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD std::uint32_t numberOfOrders() const SBE_NOEXCEPT
        {
            std::uint32_t val;
            std::memcpy(&val, m_buffer + m_offset + 57, sizeof(std::uint32_t));
            return SBE_BIG_ENDIAN_ENCODE_32(val);
        }

        MDIncGrp &numberOfOrders(const std::uint32_t value) SBE_NOEXCEPT
        {
            std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
            std::memcpy(m_buffer + m_offset + 57, &val, sizeof(std::uint32_t));
            return *this;
        }

        SBE_NODISCARD static const char *MDEntryPositionNoMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryPositionNoId() SBE_NOEXCEPT
        {
            return 290;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryPositionNoSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryPositionNoInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryPositionNoEncodingOffset() SBE_NOEXCEPT
        {
            return 61;
        }

        static SBE_CONSTEXPR std::uint32_t mDEntryPositionNoNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_UINT32;
        }

        static SBE_CONSTEXPR std::uint32_t mDEntryPositionNoMinValue() SBE_NOEXCEPT
        {
            return UINT32_C(0x0);
        }

        static SBE_CONSTEXPR std::uint32_t mDEntryPositionNoMaxValue() SBE_NOEXCEPT
        {
            return UINT32_C(0xfffffffe);
        }

        static SBE_CONSTEXPR std::size_t mDEntryPositionNoEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD std::uint32_t mDEntryPositionNo() const SBE_NOEXCEPT
        {
            std::uint32_t val;
            std::memcpy(&val, m_buffer + m_offset + 61, sizeof(std::uint32_t));
            return SBE_BIG_ENDIAN_ENCODE_32(val);
        }

        MDIncGrp &mDEntryPositionNo(const std::uint32_t value) SBE_NOEXCEPT
        {
            std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
            std::memcpy(m_buffer + m_offset + 61, &val, sizeof(std::uint32_t));
            return *this;
        }

        SBE_NODISCARD static const char *MDPriceLevelMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDPriceLevelId() SBE_NOEXCEPT
        {
            return 1023;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDPriceLevelSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDPriceLevelInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDPriceLevelEncodingOffset() SBE_NOEXCEPT
        {
            return 65;
        }

        static SBE_CONSTEXPR std::uint32_t mDPriceLevelNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_UINT32;
        }

        static SBE_CONSTEXPR std::uint32_t mDPriceLevelMinValue() SBE_NOEXCEPT
        {
            return UINT32_C(0x0);
        }

        static SBE_CONSTEXPR std::uint32_t mDPriceLevelMaxValue() SBE_NOEXCEPT
        {
            return UINT32_C(0xfffffffe);
        }

        static SBE_CONSTEXPR std::size_t mDPriceLevelEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD std::uint32_t mDPriceLevel() const SBE_NOEXCEPT
        {
            std::uint32_t val;
            std::memcpy(&val, m_buffer + m_offset + 65, sizeof(std::uint32_t));
            return SBE_BIG_ENDIAN_ENCODE_32(val);
        }

        MDIncGrp &mDPriceLevel(const std::uint32_t value) SBE_NOEXCEPT
        {
            std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
            std::memcpy(m_buffer + m_offset + 65, &val, sizeof(std::uint32_t));
            return *this;
        }

        SBE_NODISCARD static const char *AggressorSideMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "char";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t aggressorSideId() SBE_NOEXCEPT
        {
            return 5797;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t aggressorSideSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool aggressorSideInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t aggressorSideEncodingOffset() SBE_NOEXCEPT
        {
            return 69;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t aggressorSideEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD char aggressorSideRaw() const SBE_NOEXCEPT
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 69, sizeof(char));
            return (val);
        }

        SBE_NODISCARD AggressorSideEnum::Value aggressorSide() const
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 69, sizeof(char));
            return AggressorSideEnum::get((val));
        }

        MDIncGrp &aggressorSide(const AggressorSideEnum::Value value) SBE_NOEXCEPT
        {
            char val = (value);
            std::memcpy(m_buffer + m_offset + 69, &val, sizeof(char));
            return *this;
        }

        SBE_NODISCARD static const char *ScreenedMDTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t screenedMDTypeId() SBE_NOEXCEPT
        {
            return 275;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t screenedMDTypeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool screenedMDTypeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t screenedMDTypeEncodingOffset() SBE_NOEXCEPT
        {
            return 70;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t screenedMDTypeEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD std::uint8_t screenedMDTypeRaw() const SBE_NOEXCEPT
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 70, sizeof(std::uint8_t));
            return (val);
        }

        SBE_NODISCARD ScreenedMDRequestTypeEnum::Value screenedMDType() const
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 70, sizeof(std::uint8_t));
            return ScreenedMDRequestTypeEnum::get((val));
        }

        MDIncGrp &screenedMDType(const ScreenedMDRequestTypeEnum::Value value) SBE_NOEXCEPT
        {
            std::uint8_t val = (value);
            std::memcpy(m_buffer + m_offset + 70, &val, sizeof(std::uint8_t));
            return *this;
        }

        SBE_NODISCARD static const char *OrdPFillMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "Boolean";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t ordPFillId() SBE_NOEXCEPT
        {
            return 276;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t ordPFillSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool ordPFillInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t ordPFillEncodingOffset() SBE_NOEXCEPT
        {
            return 71;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t ordPFillEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD std::uint8_t ordPFillRaw() const SBE_NOEXCEPT
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 71, sizeof(std::uint8_t));
            return (val);
        }

        SBE_NODISCARD BooleanEnum::Value ordPFill() const
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 71, sizeof(std::uint8_t));
            return BooleanEnum::get((val));
        }

        MDIncGrp &ordPFill(const BooleanEnum::Value value) SBE_NOEXCEPT
        {
            std::uint8_t val = (value);
            std::memcpy(m_buffer + m_offset + 71, &val, sizeof(std::uint8_t));
            return *this;
        }

        SBE_NODISCARD static const char *MDEntryCalculationPeriodMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryCalculationPeriodId() SBE_NOEXCEPT
        {
            return 30802;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryCalculationPeriodSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryCalculationPeriodInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryCalculationPeriodEncodingOffset() SBE_NOEXCEPT
        {
            return 72;
        }

        static SBE_CONSTEXPR std::int32_t mDEntryCalculationPeriodNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_INT32;
        }

        static SBE_CONSTEXPR std::int32_t mDEntryCalculationPeriodMinValue() SBE_NOEXCEPT
        {
            return INT32_C(-2147483647);
        }

        static SBE_CONSTEXPR std::int32_t mDEntryCalculationPeriodMaxValue() SBE_NOEXCEPT
        {
            return INT32_C(2147483647);
        }

        static SBE_CONSTEXPR std::size_t mDEntryCalculationPeriodEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD std::int32_t mDEntryCalculationPeriod() const SBE_NOEXCEPT
        {
            std::int32_t val;
            std::memcpy(&val, m_buffer + m_offset + 72, sizeof(std::int32_t));
            return SBE_BIG_ENDIAN_ENCODE_32(val);
        }

        MDIncGrp &mDEntryCalculationPeriod(const std::int32_t value) SBE_NOEXCEPT
        {
            std::int32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
            std::memcpy(m_buffer + m_offset + 72, &val, sizeof(std::int32_t));
            return *this;
        }

        SBE_NODISCARD static const char *MDEntryClosingTimeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "UTCTimestamp";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t mDEntryClosingTimeId() SBE_NOEXCEPT
        {
            return 30803;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryClosingTimeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool mDEntryClosingTimeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryClosingTimeEncodingOffset() SBE_NOEXCEPT
        {
            return 76;
        }

private:
        TimestampEncoding m_mDEntryClosingTime;

public:
        SBE_NODISCARD TimestampEncoding &mDEntryClosingTime()
        {
            m_mDEntryClosingTime.wrap(m_buffer, m_offset + 76, m_actingVersion, m_bufferLength);
            return m_mDEntryClosingTime;
        }

        SBE_NODISCARD static const char *CreditCodeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t creditCodeId() SBE_NOEXCEPT
        {
            return 30805;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t creditCodeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool creditCodeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t creditCodeEncodingOffset() SBE_NOEXCEPT
        {
            return 84;
        }

        static SBE_CONSTEXPR char creditCodeNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char creditCodeMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char creditCodeMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t creditCodeEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        static SBE_CONSTEXPR std::uint64_t creditCodeLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD const char *creditCode() const SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 84;
        }

        SBE_NODISCARD char *creditCode() SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 84;
        }

        SBE_NODISCARD char creditCode(const std::uint64_t index) const
        {
            if (index >= 4)
            {
                throw std::runtime_error("index out of range for creditCode [E104]");
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 84 + (index * 1), sizeof(char));
            return (val);
        }

        MDIncGrp &creditCode(const std::uint64_t index, const char value)
        {
            if (index >= 4)
            {
                throw std::runtime_error("index out of range for creditCode [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 84 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getCreditCode(char *const dst, const std::uint64_t length) const
        {
            if (length > 4)
            {
                throw std::runtime_error("length too large for getCreditCode [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 84, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getCreditCodeAsSpan() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 84;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 4);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        MDIncGrp &putCreditCode(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 4, "array too large for putCreditCode");

            std::memcpy(m_buffer + m_offset + 84, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 4; ++start)
            {
                m_buffer[m_offset + 84 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        MDIncGrp &putCreditCode(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        MDIncGrp &putCreditCode(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 84, src, sizeof(char) * 4);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        MDIncGrp &putCreditCode(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putCreditCode(std::span<const char, N>(src));
        }
        #endif

        MDIncGrp &putCreditCode(
            const char value0,
            const char value1,
            const char value2,
            const char value3) SBE_NOEXCEPT
        {
            char val0 = (value0);
            std::memcpy(m_buffer + m_offset + 84, &val0, sizeof(char));
            char val1 = (value1);
            std::memcpy(m_buffer + m_offset + 85, &val1, sizeof(char));
            char val2 = (value2);
            std::memcpy(m_buffer + m_offset + 86, &val2, sizeof(char));
            char val3 = (value3);
            std::memcpy(m_buffer + m_offset + 87, &val3, sizeof(char));

            return *this;
        }

        SBE_NODISCARD std::string getCreditCodeAsString() const
        {
            const char *buffer = m_buffer + m_offset + 84;
            std::size_t length = 0;

            for (; length < 4 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getCreditCodeAsJsonEscapedString()
        {
            std::ostringstream oss;
            std::string s = getCreditCodeAsString();

            for (const auto c : s)
            {
                switch (c)
                {
                    case '"': oss << "\\\""; break;
                    case '\\': oss << "\\\\"; break;
                    case '\b': oss << "\\b"; break;
                    case '\f': oss << "\\f"; break;
                    case '\n': oss << "\\n"; break;
                    case '\r': oss << "\\r"; break;
                    case '\t': oss << "\\t"; break;

                    default:
                        if ('\x00' <= c && c <= '\x1f')
                        {
                            oss << "\\u" << std::hex << std::setw(4)
                                << std::setfill('0') << (int)(c);
                        }
                        else
                        {
                            oss << c;
                        }
                }
            }

            return oss.str();
        }

        #ifdef SBE_USE_STRING_VIEW
        SBE_NODISCARD std::string_view getCreditCodeAsStringView() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 84;
            std::size_t length = 0;

            for (; length < 4 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        MDIncGrp &putCreditCode(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 4)
            {
                throw std::runtime_error("string too large for putCreditCode [E106]");
            }

            std::memcpy(m_buffer + m_offset + 84, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 4; ++start)
            {
                m_buffer[m_offset + 84 + start] = 0;
            }

            return *this;
        }
        #else
        MDIncGrp &putCreditCode(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 4)
            {
                throw std::runtime_error("string too large for putCreditCode [E106]");
            }

            std::memcpy(m_buffer + m_offset + 84, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 4; ++start)
            {
                m_buffer[m_offset + 84 + start] = 0;
            }

            return *this;
        }
        #endif

        class Parties
        {
        private:
            char *m_buffer = nullptr;
            std::uint64_t m_bufferLength = 0;
            std::uint64_t m_initialPosition = 0;
            std::uint64_t *m_positionPtr = nullptr;
            std::uint64_t m_blockLength = 0;
            std::uint64_t m_count = 0;
            std::uint64_t m_index = 0;
            std::uint64_t m_offset = 0;
            std::uint64_t m_actingVersion = 0;

            SBE_NODISCARD std::uint64_t *sbePositionPtr() SBE_NOEXCEPT
            {
                return m_positionPtr;
            }

        public:
            Parties() = default;

            inline void wrapForDecode(
                char *buffer,
                std::uint64_t *pos,
                const std::uint64_t actingVersion,
                const std::uint64_t bufferLength)
            {
                GroupSizeEncoding dimensions(buffer, *pos, bufferLength, actingVersion);
                m_buffer = buffer;
                m_bufferLength = bufferLength;
                m_blockLength = dimensions.blockLength();
                m_count = dimensions.numInGroup();
                m_index = 0;
                m_actingVersion = actingVersion;
                m_initialPosition = *pos;
                m_positionPtr = pos;
                *m_positionPtr = *m_positionPtr + 8;
            }

            inline void wrapForEncode(
                char *buffer,
                const std::uint16_t count,
                std::uint64_t *pos,
                const std::uint64_t actingVersion,
                const std::uint64_t bufferLength)
            {
        #if defined(__GNUG__) && !defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wtype-limits"
        #endif
                if (count > 65534)
                {
                    throw std::runtime_error("count outside of allowed range [E110]");
                }
        #if defined(__GNUG__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif
                m_buffer = buffer;
                m_bufferLength = bufferLength;
                GroupSizeEncoding dimensions(buffer, *pos, bufferLength, actingVersion);
                dimensions.blockLength(static_cast<std::uint16_t>(10));
                dimensions.numInGroup(static_cast<std::uint16_t>(count));
                m_index = 0;
                m_count = count;
                m_blockLength = 10;
                m_actingVersion = actingVersion;
                m_initialPosition = *pos;
                m_positionPtr = pos;
                *m_positionPtr = *m_positionPtr + 8;
            }

            static SBE_CONSTEXPR std::uint64_t sbeHeaderSize() SBE_NOEXCEPT
            {
                return 8;
            }

            static SBE_CONSTEXPR std::uint64_t sbeBlockLength() SBE_NOEXCEPT
            {
                return 10;
            }

            SBE_NODISCARD std::uint64_t sbeActingBlockLength() SBE_NOEXCEPT
            {
                return m_blockLength;
            }

            SBE_NODISCARD std::uint64_t sbePosition() const SBE_NOEXCEPT
            {
                return *m_positionPtr;
            }

            // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
            std::uint64_t sbeCheckPosition(const std::uint64_t position)
            {
                if (SBE_BOUNDS_CHECK_EXPECT((position > m_bufferLength), false))
                {
                    throw std::runtime_error("buffer too short [E100]");
                }
                return position;
            }

            void sbePosition(const std::uint64_t position)
            {
                *m_positionPtr = sbeCheckPosition(position);
            }

            SBE_NODISCARD inline std::uint64_t count() const SBE_NOEXCEPT
            {
                return m_count;
            }

            SBE_NODISCARD inline bool hasNext() const SBE_NOEXCEPT
            {
                return m_index < m_count;
            }

            inline Parties &next()
            {
                if (m_index >= m_count)
                {
                    throw std::runtime_error("index >= count [E108]");
                }
                m_offset = *m_positionPtr;
                if (SBE_BOUNDS_CHECK_EXPECT(((m_offset + m_blockLength) > m_bufferLength), false))
                {
                    throw std::runtime_error("buffer too short for next group index [E108]");
                }
                *m_positionPtr = m_offset + m_blockLength;
                ++m_index;

                return *this;
            }

            inline std::uint64_t resetCountToIndex()
            {
                m_count = m_index;
                GroupSizeEncoding dimensions(m_buffer, m_initialPosition, m_bufferLength, m_actingVersion);
                dimensions.numInGroup(static_cast<std::uint16_t>(m_count));
                return m_count;
            }

            template<class Func> inline void forEach(Func &&func)
            {
                while (hasNext())
                {
                    next();
                    func(*this);
                }
            }


            SBE_NODISCARD static const char *PartyIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
            {
                switch (metaAttribute)
                {
                    case MetaAttribute::SEMANTIC_TYPE: return "String";
                    case MetaAttribute::PRESENCE: return "optional";
                    default: return "";
                }
            }

            static SBE_CONSTEXPR std::uint16_t partyIDId() SBE_NOEXCEPT
            {
                return 448;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t partyIDSinceVersion() SBE_NOEXCEPT
            {
                return 0;
            }

            SBE_NODISCARD bool partyIDInActingVersion() SBE_NOEXCEPT
            {
                return true;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::size_t partyIDEncodingOffset() SBE_NOEXCEPT
            {
                return 0;
            }

            static SBE_CONSTEXPR char partyIDNullValue() SBE_NOEXCEPT
            {
                return static_cast<char>(0);
            }

            static SBE_CONSTEXPR char partyIDMinValue() SBE_NOEXCEPT
            {
                return static_cast<char>(32);
            }

            static SBE_CONSTEXPR char partyIDMaxValue() SBE_NOEXCEPT
            {
                return static_cast<char>(126);
            }

            static SBE_CONSTEXPR std::size_t partyIDEncodingLength() SBE_NOEXCEPT
            {
                return 8;
            }

            static SBE_CONSTEXPR std::uint64_t partyIDLength() SBE_NOEXCEPT
            {
                return 8;
            }

            SBE_NODISCARD const char *partyID() const SBE_NOEXCEPT
            {
                return m_buffer + m_offset + 0;
            }

            SBE_NODISCARD char *partyID() SBE_NOEXCEPT
            {
                return m_buffer + m_offset + 0;
            }

            SBE_NODISCARD char partyID(const std::uint64_t index) const
            {
                if (index >= 8)
                {
                    throw std::runtime_error("index out of range for partyID [E104]");
                }

                char val;
                std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
                return (val);
            }

            Parties &partyID(const std::uint64_t index, const char value)
            {
                if (index >= 8)
                {
                    throw std::runtime_error("index out of range for partyID [E105]");
                }

                char val = (value);
                std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
                return *this;
            }

            std::uint64_t getPartyID(char *const dst, const std::uint64_t length) const
            {
                if (length > 8)
                {
                    throw std::runtime_error("length too large for getPartyID [E106]");
                }

                std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
                return length;
            }

            #ifdef SBE_USE_SPAN
            SBE_NODISCARD std::span<const char> getPartyIDAsSpan() const SBE_NOEXCEPT
            {
                const char *buffer = m_buffer + m_offset + 0;
                return std::span<const char>(reinterpret_cast<const char*>(buffer), 8);
            }
            #endif

            #ifdef SBE_USE_SPAN
            template <std::size_t N>
            Parties &putPartyID(std::span<const char, N> src) SBE_NOEXCEPT
            {
                static_assert(N <= 8, "array too large for putPartyID");

                std::memcpy(m_buffer + m_offset + 0, src.data(), sizeof(char) * N);
                for (std::size_t start = N; start < 8; ++start)
                {
                    m_buffer[m_offset + 0 + start] = 0;
                }

                return *this;
            }
            #endif

            #ifdef SBE_USE_SPAN
            template <typename T>
            Parties &putPartyID(T&& src)  SBE_NOEXCEPT requires
                (std::is_pointer_v<std::remove_reference_t<T>> &&
                 !std::is_array_v<std::remove_reference_t<T>>)
            #else
            Parties &putPartyID(const char *const src) SBE_NOEXCEPT
            #endif
            {
                std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 8);
                return *this;
            }

            #ifdef SBE_USE_SPAN
            template <std::size_t N>
            Parties &putPartyID(const char (&src)[N]) SBE_NOEXCEPT
            {
                return putPartyID(std::span<const char, N>(src));
            }
            #endif

            SBE_NODISCARD std::string getPartyIDAsString() const
            {
                const char *buffer = m_buffer + m_offset + 0;
                std::size_t length = 0;

                for (; length < 8 && *(buffer + length) != '\0'; ++length);
                std::string result(buffer, length);

                return result;
            }

            std::string getPartyIDAsJsonEscapedString()
            {
                std::ostringstream oss;
                std::string s = getPartyIDAsString();

                for (const auto c : s)
                {
                    switch (c)
                    {
                        case '"': oss << "\\\""; break;
                        case '\\': oss << "\\\\"; break;
                        case '\b': oss << "\\b"; break;
                        case '\f': oss << "\\f"; break;
                        case '\n': oss << "\\n"; break;
                        case '\r': oss << "\\r"; break;
                        case '\t': oss << "\\t"; break;

                        default:
                            if ('\x00' <= c && c <= '\x1f')
                            {
                                oss << "\\u" << std::hex << std::setw(4)
                                    << std::setfill('0') << (int)(c);
                            }
                            else
                            {
                                oss << c;
                            }
                    }
                }

                return oss.str();
            }

            #ifdef SBE_USE_STRING_VIEW
            SBE_NODISCARD std::string_view getPartyIDAsStringView() const SBE_NOEXCEPT
            {
                const char *buffer = m_buffer + m_offset + 0;
                std::size_t length = 0;

                for (; length < 8 && *(buffer + length) != '\0'; ++length);
                std::string_view result(buffer, length);

                return result;
            }
            #endif

            #ifdef SBE_USE_STRING_VIEW
            Parties &putPartyID(const std::string_view str)
            {
                const std::size_t srcLength = str.length();
                if (srcLength > 8)
                {
                    throw std::runtime_error("string too large for putPartyID [E106]");
                }

                std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
                for (std::size_t start = srcLength; start < 8; ++start)
                {
                    m_buffer[m_offset + 0 + start] = 0;
                }

                return *this;
            }
            #else
            Parties &putPartyID(const std::string &str)
            {
                const std::size_t srcLength = str.length();
                if (srcLength > 8)
                {
                    throw std::runtime_error("string too large for putPartyID [E106]");
                }

                std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
                for (std::size_t start = srcLength; start < 8; ++start)
                {
                    m_buffer[m_offset + 0 + start] = 0;
                }

                return *this;
            }
            #endif

            SBE_NODISCARD static const char *PartyIDSourceMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
            {
                switch (metaAttribute)
                {
                    case MetaAttribute::SEMANTIC_TYPE: return "char";
                    case MetaAttribute::PRESENCE: return "optional";
                    default: return "";
                }
            }

            static SBE_CONSTEXPR std::uint16_t partyIDSourceId() SBE_NOEXCEPT
            {
                return 447;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t partyIDSourceSinceVersion() SBE_NOEXCEPT
            {
                return 0;
            }

            SBE_NODISCARD bool partyIDSourceInActingVersion() SBE_NOEXCEPT
            {
                return true;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::size_t partyIDSourceEncodingOffset() SBE_NOEXCEPT
            {
                return 8;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::size_t partyIDSourceEncodingLength() SBE_NOEXCEPT
            {
                return 1;
            }

            SBE_NODISCARD char partyIDSourceRaw() const SBE_NOEXCEPT
            {
                char val;
                std::memcpy(&val, m_buffer + m_offset + 8, sizeof(char));
                return (val);
            }

            SBE_NODISCARD PartyIDSourceEnum::Value partyIDSource() const
            {
                char val;
                std::memcpy(&val, m_buffer + m_offset + 8, sizeof(char));
                return PartyIDSourceEnum::get((val));
            }

            Parties &partyIDSource(const PartyIDSourceEnum::Value value) SBE_NOEXCEPT
            {
                char val = (value);
                std::memcpy(m_buffer + m_offset + 8, &val, sizeof(char));
                return *this;
            }

            SBE_NODISCARD static const char *PartyRoleMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
            {
                switch (metaAttribute)
                {
                    case MetaAttribute::SEMANTIC_TYPE: return "int";
                    case MetaAttribute::PRESENCE: return "optional";
                    default: return "";
                }
            }

            static SBE_CONSTEXPR std::uint16_t partyRoleId() SBE_NOEXCEPT
            {
                return 452;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t partyRoleSinceVersion() SBE_NOEXCEPT
            {
                return 0;
            }

            SBE_NODISCARD bool partyRoleInActingVersion() SBE_NOEXCEPT
            {
                return true;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::size_t partyRoleEncodingOffset() SBE_NOEXCEPT
            {
                return 9;
            }

            SBE_NODISCARD static SBE_CONSTEXPR std::size_t partyRoleEncodingLength() SBE_NOEXCEPT
            {
                return 1;
            }

            SBE_NODISCARD std::uint8_t partyRoleRaw() const SBE_NOEXCEPT
            {
                std::uint8_t val;
                std::memcpy(&val, m_buffer + m_offset + 9, sizeof(std::uint8_t));
                return (val);
            }

            SBE_NODISCARD PartyRoleEnum::Value partyRole() const
            {
                std::uint8_t val;
                std::memcpy(&val, m_buffer + m_offset + 9, sizeof(std::uint8_t));
                return PartyRoleEnum::get((val));
            }

            Parties &partyRole(const PartyRoleEnum::Value value) SBE_NOEXCEPT
            {
                std::uint8_t val = (value);
                std::memcpy(m_buffer + m_offset + 9, &val, sizeof(std::uint8_t));
                return *this;
            }

            template<typename CharT, typename Traits>
            friend std::basic_ostream<CharT, Traits> & operator << (
                std::basic_ostream<CharT, Traits> &builder, Parties &writer)
            {
                builder << '{';
                builder << R"("PartyID": )";
                builder << '"' <<
                    writer.getPartyIDAsJsonEscapedString().c_str() << '"';

                builder << ", ";
                builder << R"("PartyIDSource": )";
                builder << '"' << writer.partyIDSource() << '"';

                builder << ", ";
                builder << R"("PartyRole": )";
                builder << '"' << writer.partyRole() << '"';

                builder << '}';

                return builder;
            }

            void skip()
            {
            }

            SBE_NODISCARD static SBE_CONSTEXPR bool isConstLength() SBE_NOEXCEPT
            {
                return true;
            }

            SBE_NODISCARD static std::size_t computeLength()
            {
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
                std::size_t length = sbeBlockLength();

                return length;
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
            }
        };

private:
        Parties m_parties;

public:
        SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t PartiesId() SBE_NOEXCEPT
        {
            return 35008;
        }

        SBE_NODISCARD inline Parties &parties()
        {
            m_parties.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
            return m_parties;
        }

        Parties &partiesCount(const std::uint16_t count)
        {
            m_parties.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
            return m_parties;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t partiesSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool partiesInActingVersion() const SBE_NOEXCEPT
        {
            return true;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, MDIncGrp &writer)
        {
            builder << '{';
            builder << R"("MDUpdateAction": )";
            builder << '"' << writer.mDUpdateAction() << '"';

            builder << ", ";
            builder << R"("MDSubBookType": )";
            builder << +writer.mDSubBookType();

            builder << ", ";
            builder << R"("MDEntryType": )";
            builder << '"' << writer.mDEntryType() << '"';

            builder << ", ";
            builder << R"("MDEntryID": )";
            builder << '"' <<
                writer.getMDEntryIDAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("SecurityID": )";
            builder << '"' <<
                writer.getSecurityIDAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("MDEntryPx": )";
            builder << writer.mDEntryPx();

            builder << ", ";
            builder << R"("MDEntrySize": )";
            builder << writer.mDEntrySize();

            builder << ", ";
            builder << R"("MDEntryTime": )";
            builder << writer.mDEntryTime();

            builder << ", ";
            builder << R"("NumberOfOrders": )";
            builder << +writer.numberOfOrders();

            builder << ", ";
            builder << R"("MDEntryPositionNo": )";
            builder << +writer.mDEntryPositionNo();

            builder << ", ";
            builder << R"("MDPriceLevel": )";
            builder << +writer.mDPriceLevel();

            builder << ", ";
            builder << R"("AggressorSide": )";
            builder << '"' << writer.aggressorSide() << '"';

            builder << ", ";
            builder << R"("ScreenedMDType": )";
            builder << '"' << writer.screenedMDType() << '"';

            builder << ", ";
            builder << R"("OrdPFill": )";
            builder << '"' << writer.ordPFill() << '"';

            builder << ", ";
            builder << R"("MDEntryCalculationPeriod": )";
            builder << +writer.mDEntryCalculationPeriod();

            builder << ", ";
            builder << R"("MDEntryClosingTime": )";
            builder << writer.mDEntryClosingTime();

            builder << ", ";
            builder << R"("CreditCode": )";
            builder << '"' <<
                writer.getCreditCodeAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            {
                bool atLeastOne = false;
                builder << R"("Parties": [)";
                writer.parties().forEach(
                    [&](Parties &parties)
                    {
                        if (atLeastOne)
                        {
                            builder << ", ";
                        }
                        atLeastOne = true;
                        builder << parties;
                    });
                builder << ']';
            }

            builder << '}';

            return builder;
        }

        void skip()
        {
            auto &partiesGroup { parties() };
            while (partiesGroup.hasNext())
            {
                partiesGroup.next().skip();
            }
        }

        SBE_NODISCARD static SBE_CONSTEXPR bool isConstLength() SBE_NOEXCEPT
        {
            return false;
        }

        SBE_NODISCARD static std::size_t computeLength(std::size_t partiesLength = 0)
        {
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
            std::size_t length = sbeBlockLength();

            length += Parties::sbeHeaderSize();
            if (partiesLength > 65534LL)
            {
                throw std::runtime_error("partiesLength outside of allowed range [E110]");
            }
            length += partiesLength *Parties::sbeBlockLength();

            return length;
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        }
    };

private:
    MDIncGrp m_mDIncGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t MDIncGrpId() SBE_NOEXCEPT
    {
        return 35007;
    }

    SBE_NODISCARD inline MDIncGrp &mDIncGrp()
    {
        m_mDIncGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_mDIncGrp;
    }

    MDIncGrp &mDIncGrpCount(const std::uint16_t count)
    {
        m_mDIncGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_mDIncGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDIncGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool mDIncGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, const MarketDataIncrementalRefresh &_writer)
{
    MarketDataIncrementalRefresh writer(
        _writer.m_buffer,
        _writer.m_offset,
        _writer.m_bufferLength,
        _writer.m_actingBlockLength,
        _writer.m_actingVersion);

    builder << '{';
    builder << R"("Name": "MarketDataIncrementalRefresh", )";
    builder << R"("sbeTemplateId": )";
    builder << writer.sbeTemplateId();
    builder << ", ";

    builder << R"("ApplID": )";
    builder << '"' <<
        writer.getApplIDAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("BookStateID": )";
    builder << +writer.bookStateID();

    builder << ", ";
    builder << R"("ConflationID": )";
    builder << +writer.conflationID();

    builder << ", ";
    builder << R"("CreditStateID": )";
    builder << +writer.creditStateID();

    builder << ", ";
    builder << R"("MessageEventSource": )";
    builder << '"' << writer.messageEventSource() << '"';

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("MDIncGrp": [)";
        writer.mDIncGrp().forEach(
            [&](MDIncGrp &mDIncGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << mDIncGrp;
            });
        builder << ']';
    }

    builder << '}';

    return builder;
}

void skip()
{
    auto &mDIncGrpGroup { mDIncGrp() };
    while (mDIncGrpGroup.hasNext())
    {
        mDIncGrpGroup.next().skip();
    }
}

SBE_NODISCARD static SBE_CONSTEXPR bool isConstLength() SBE_NOEXCEPT
{
    return false;
}

SBE_NODISCARD static std::size_t computeLength(const std::vector<std::tuple<std::size_t>> &mDIncGrpItemLengths = {})
{
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    std::size_t length = sbeBlockLength();

    length += MDIncGrp::sbeHeaderSize();
    if (mDIncGrpItemLengths.size() > 65534LL)
    {
        throw std::runtime_error("mDIncGrpItemLengths.size() outside of allowed range [E110]");
    }

    for (const auto &e: mDIncGrpItemLengths)
    {
        #if __cplusplus >= 201703L
        length += std::apply(MDIncGrp::computeLength, e);
        #else
        length += MDIncGrp::computeLength(std::get<0>(e));
        #endif
    }

    return length;
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
};
}
#endif
