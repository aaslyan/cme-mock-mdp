/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_SECURITYDEFINITIONREQUEST_CXX_H_
#define _LSEG_SBE_SECURITYDEFINITIONREQUEST_CXX_H_

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

class SecurityDefinitionRequest
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
    static constexpr std::uint16_t SBE_BLOCK_LENGTH = static_cast<std::uint16_t>(10);
    static constexpr std::uint16_t SBE_TEMPLATE_ID = static_cast<std::uint16_t>(2);
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

    SecurityDefinitionRequest() = default;

    SecurityDefinitionRequest(
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

    SecurityDefinitionRequest(char *buffer, const std::uint64_t bufferLength) :
        SecurityDefinitionRequest(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion())
    {
    }

    SecurityDefinitionRequest(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        SecurityDefinitionRequest(buffer, 0, bufferLength, actingBlockLength, actingVersion)
    {
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeBlockLength() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(10);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sbeBlockAndHeaderLength() SBE_NOEXCEPT
    {
        return messageHeader::encodedLength() + sbeBlockLength();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeTemplateId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(2);
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
        return "c";
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    SecurityDefinitionRequest &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingBlockLength = sbeBlockLength();
        m_actingVersion = sbeSchemaVersion();
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    SecurityDefinitionRequest &wrapAndApplyHeader(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
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

    SecurityDefinitionRequest &wrapForDecode(
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

    SecurityDefinitionRequest &sbeRewind()
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
        SecurityDefinitionRequest skipper(m_buffer, m_offset, m_bufferLength, m_actingBlockLength, m_actingVersion);
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

    SBE_NODISCARD static const char *SecurityReqIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t securityReqIDId() SBE_NOEXCEPT
    {
        return 320;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityReqIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool securityReqIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityReqIDEncodingOffset() SBE_NOEXCEPT
    {
        return 0;
    }

    static SBE_CONSTEXPR char securityReqIDNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char securityReqIDMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char securityReqIDMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t securityReqIDEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t securityReqIDLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *securityReqID() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char *securityReqID() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char securityReqID(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for securityReqID [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
        return (val);
    }

    SecurityDefinitionRequest &securityReqID(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for securityReqID [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSecurityReqID(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getSecurityReqID [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getSecurityReqIDAsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 8);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinitionRequest &putSecurityReqID(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 8, "array too large for putSecurityReqID");

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
    SecurityDefinitionRequest &putSecurityReqID(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinitionRequest &putSecurityReqID(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 8);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinitionRequest &putSecurityReqID(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putSecurityReqID(std::span<const char, N>(src));
    }
    #endif

    SBE_NODISCARD std::string getSecurityReqIDAsString() const
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSecurityReqIDAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getSecurityReqIDAsString();

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
    SBE_NODISCARD std::string_view getSecurityReqIDAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    SecurityDefinitionRequest &putSecurityReqID(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSecurityReqID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #else
    SecurityDefinitionRequest &putSecurityReqID(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSecurityReqID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *SubscriptionRequestTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "char";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t subscriptionRequestTypeId() SBE_NOEXCEPT
    {
        return 263;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t subscriptionRequestTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool subscriptionRequestTypeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t subscriptionRequestTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t subscriptionRequestTypeEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD char subscriptionRequestTypeRaw() const SBE_NOEXCEPT
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 8, sizeof(char));
        return (val);
    }

    SBE_NODISCARD SubscriptionRequestTypeEnumSecDef::Value subscriptionRequestType() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 8, sizeof(char));
        return SubscriptionRequestTypeEnumSecDef::get((val));
    }

    SecurityDefinitionRequest &subscriptionRequestType(const SubscriptionRequestTypeEnumSecDef::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 8, &val, sizeof(char));
        return *this;
    }

    SBE_NODISCARD static const char *SecurityRequestTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t securityRequestTypeId() SBE_NOEXCEPT
    {
        return 321;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityRequestTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool securityRequestTypeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityRequestTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 9;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityRequestTypeEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD char securityRequestTypeRaw() const SBE_NOEXCEPT
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 9, sizeof(char));
        return (val);
    }

    SBE_NODISCARD SecurityRequestTypeEnum::Value securityRequestType() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 9, sizeof(char));
        return SecurityRequestTypeEnum::get((val));
    }

    SecurityDefinitionRequest &securityRequestType(const SecurityRequestTypeEnum::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 9, &val, sizeof(char));
        return *this;
    }

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, const SecurityDefinitionRequest &_writer)
{
    SecurityDefinitionRequest writer(
        _writer.m_buffer,
        _writer.m_offset,
        _writer.m_bufferLength,
        _writer.m_actingBlockLength,
        _writer.m_actingVersion);

    builder << '{';
    builder << R"("Name": "SecurityDefinitionRequest", )";
    builder << R"("sbeTemplateId": )";
    builder << writer.sbeTemplateId();
    builder << ", ";

    builder << R"("SecurityReqID": )";
    builder << '"' <<
        writer.getSecurityReqIDAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("SubscriptionRequestType": )";
    builder << '"' << writer.subscriptionRequestType() << '"';

    builder << ", ";
    builder << R"("SecurityRequestType": )";
    builder << '"' << writer.securityRequestType() << '"';

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
