/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_ESTABLISH_CXX_H_
#define _LSEG_SBE_ESTABLISH_CXX_H_

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

class Establish
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
    static constexpr std::uint16_t SBE_TEMPLATE_ID = static_cast<std::uint16_t>(501);
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

    Establish() = default;

    Establish(
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

    Establish(char *buffer, const std::uint64_t bufferLength) :
        Establish(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion())
    {
    }

    Establish(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        Establish(buffer, 0, bufferLength, actingBlockLength, actingVersion)
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
        return static_cast<std::uint16_t>(501);
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
        return "A";
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    Establish &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingBlockLength = sbeBlockLength();
        m_actingVersion = sbeSchemaVersion();
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    Establish &wrapAndApplyHeader(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
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

    Establish &wrapForDecode(
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

    Establish &sbeRewind()
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
        Establish skipper(m_buffer, m_offset, m_bufferLength, m_actingBlockLength, m_actingVersion);
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

    SBE_NODISCARD static const char *MessageTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Enum";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t messageTypeId() SBE_NOEXCEPT
    {
        return 40001;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t messageTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool messageTypeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t messageTypeEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD char messageTypeRaw() const SBE_NOEXCEPT
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 0, sizeof(char));
        return (val);
    }

    SBE_NODISCARD MessageTypeEnum::Value messageType() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 0, sizeof(char));
        return MessageTypeEnum::get((val));
    }

    Establish &messageType(const MessageTypeEnum::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 0, &val, sizeof(char));
        return *this;
    }

    SBE_NODISCARD static const char *SessionIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "UUID";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t sessionIDId() SBE_NOEXCEPT
    {
        return 40002;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sessionIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool sessionIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t sessionIDEncodingOffset() SBE_NOEXCEPT
    {
        return 1;
    }

    static SBE_CONSTEXPR char sessionIDNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char sessionIDMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char sessionIDMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t sessionIDEncodingLength() SBE_NOEXCEPT
    {
        return 20;
    }

    static SBE_CONSTEXPR std::uint64_t sessionIDLength() SBE_NOEXCEPT
    {
        return 20;
    }

    SBE_NODISCARD const char *sessionID() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 1;
    }

    SBE_NODISCARD char *sessionID() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 1;
    }

    SBE_NODISCARD char sessionID(const std::uint64_t index) const
    {
        if (index >= 20)
        {
            throw std::runtime_error("index out of range for sessionID [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 1 + (index * 1), sizeof(char));
        return (val);
    }

    Establish &sessionID(const std::uint64_t index, const char value)
    {
        if (index >= 20)
        {
            throw std::runtime_error("index out of range for sessionID [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 1 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSessionID(char *const dst, const std::uint64_t length) const
    {
        if (length > 20)
        {
            throw std::runtime_error("length too large for getSessionID [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 1, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getSessionIDAsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 1;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 20);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    Establish &putSessionID(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 20, "array too large for putSessionID");

        std::memcpy(m_buffer + m_offset + 1, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 20; ++start)
        {
            m_buffer[m_offset + 1 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    Establish &putSessionID(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    Establish &putSessionID(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 1, src, sizeof(char) * 20);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    Establish &putSessionID(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putSessionID(std::span<const char, N>(src));
    }
    #endif

    SBE_NODISCARD std::string getSessionIDAsString() const
    {
        const char *buffer = m_buffer + m_offset + 1;
        std::size_t length = 0;

        for (; length < 20 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSessionIDAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getSessionIDAsString();

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
    SBE_NODISCARD std::string_view getSessionIDAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 1;
        std::size_t length = 0;

        for (; length < 20 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    Establish &putSessionID(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 20)
        {
            throw std::runtime_error("string too large for putSessionID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 1, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 20; ++start)
        {
            m_buffer[m_offset + 1 + start] = 0;
        }

        return *this;
    }
    #else
    Establish &putSessionID(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 20)
        {
            throw std::runtime_error("string too large for putSessionID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 1, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 20; ++start)
        {
            m_buffer[m_offset + 1 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *TimestampMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "UTCTimestamp";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t timestampId() SBE_NOEXCEPT
    {
        return 40003;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t timestampSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool timestampInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t timestampEncodingOffset() SBE_NOEXCEPT
    {
        return 21;
    }

private:
    TimestampEncoding m_timestamp;

public:
    SBE_NODISCARD TimestampEncoding &timestamp()
    {
        m_timestamp.wrap(m_buffer, m_offset + 21, m_actingVersion, m_bufferLength);
        return m_timestamp;
    }

    SBE_NODISCARD static const char *KeepAliveIntervalMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t keepAliveIntervalId() SBE_NOEXCEPT
    {
        return 40004;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t keepAliveIntervalSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool keepAliveIntervalInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t keepAliveIntervalEncodingOffset() SBE_NOEXCEPT
    {
        return 29;
    }

    static SBE_CONSTEXPR std::uint32_t keepAliveIntervalNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t keepAliveIntervalMinValue() SBE_NOEXCEPT
    {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t keepAliveIntervalMaxValue() SBE_NOEXCEPT
    {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t keepAliveIntervalEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD std::uint32_t keepAliveInterval() const SBE_NOEXCEPT
    {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 29, sizeof(std::uint32_t));
        return SBE_BIG_ENDIAN_ENCODE_32(val);
    }

    Establish &keepAliveInterval(const std::uint32_t value) SBE_NOEXCEPT
    {
        std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 29, &val, sizeof(std::uint32_t));
        return *this;
    }

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, const Establish &_writer)
{
    Establish writer(
        _writer.m_buffer,
        _writer.m_offset,
        _writer.m_bufferLength,
        _writer.m_actingBlockLength,
        _writer.m_actingVersion);

    builder << '{';
    builder << R"("Name": "Establish", )";
    builder << R"("sbeTemplateId": )";
    builder << writer.sbeTemplateId();
    builder << ", ";

    builder << R"("MessageType": )";
    builder << '"' << writer.messageType() << '"';

    builder << ", ";
    builder << R"("SessionID": )";
    builder << '"' <<
        writer.getSessionIDAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("Timestamp": )";
    builder << writer.timestamp();

    builder << ", ";
    builder << R"("KeepAliveInterval": )";
    builder << +writer.keepAliveInterval();

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
}
#endif
