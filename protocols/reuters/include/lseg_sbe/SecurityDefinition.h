/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_SECURITYDEFINITION_CXX_H_
#define _LSEG_SBE_SECURITYDEFINITION_CXX_H_

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

class SecurityDefinition
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
    static constexpr std::uint16_t SBE_BLOCK_LENGTH = static_cast<std::uint16_t>(112);
    static constexpr std::uint16_t SBE_TEMPLATE_ID = static_cast<std::uint16_t>(7);
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

    SecurityDefinition() = default;

    SecurityDefinition(
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

    SecurityDefinition(char *buffer, const std::uint64_t bufferLength) :
        SecurityDefinition(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion())
    {
    }

    SecurityDefinition(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        SecurityDefinition(buffer, 0, bufferLength, actingBlockLength, actingVersion)
    {
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeBlockLength() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(112);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sbeBlockAndHeaderLength() SBE_NOEXCEPT
    {
        return messageHeader::encodedLength() + sbeBlockLength();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeTemplateId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(7);
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
        return "d";
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    SecurityDefinition &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingBlockLength = sbeBlockLength();
        m_actingVersion = sbeSchemaVersion();
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    SecurityDefinition &wrapAndApplyHeader(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
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

    SecurityDefinition &wrapForDecode(
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

    SecurityDefinition &sbeRewind()
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
        SecurityDefinition skipper(m_buffer, m_offset, m_bufferLength, m_actingBlockLength, m_actingVersion);
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

    SecurityDefinition &applID(const std::uint64_t index, const char value)
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
    SecurityDefinition &putApplID(std::span<const char, N> src) SBE_NOEXCEPT
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
    SecurityDefinition &putApplID(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinition &putApplID(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 8);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putApplID(const char (&src)[N]) SBE_NOEXCEPT
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
    SecurityDefinition &putApplID(const std::string_view str)
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
    SecurityDefinition &putApplID(const std::string &str)
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

    SBE_NODISCARD static const char *ApplSeqNoMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "SeqNum";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t applSeqNoId() SBE_NOEXCEPT
    {
        return 1181;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t applSeqNoSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool applSeqNoInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t applSeqNoEncodingOffset() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t applSeqNoNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT64;
    }

    static SBE_CONSTEXPR std::uint64_t applSeqNoMinValue() SBE_NOEXCEPT
    {
        return UINT64_C(0x0);
    }

    static SBE_CONSTEXPR std::uint64_t applSeqNoMaxValue() SBE_NOEXCEPT
    {
        return UINT64_C(0xfffffffffffffffe);
    }

    static SBE_CONSTEXPR std::size_t applSeqNoEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD std::uint64_t applSeqNo() const SBE_NOEXCEPT
    {
        std::uint64_t val;
        std::memcpy(&val, m_buffer + m_offset + 8, sizeof(std::uint64_t));
        return SBE_BIG_ENDIAN_ENCODE_64(val);
    }

    SecurityDefinition &applSeqNo(const std::uint64_t value) SBE_NOEXCEPT
    {
        std::uint64_t val = SBE_BIG_ENDIAN_ENCODE_64(value);
        std::memcpy(m_buffer + m_offset + 8, &val, sizeof(std::uint64_t));
        return *this;
    }

    SBE_NODISCARD static const char *LastRptRequestedMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Boolean";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t lastRptRequestedId() SBE_NOEXCEPT
    {
        return 912;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t lastRptRequestedSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool lastRptRequestedInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t lastRptRequestedEncodingOffset() SBE_NOEXCEPT
    {
        return 16;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t lastRptRequestedEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t lastRptRequestedRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 16, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD BooleanEnum::Value lastRptRequested() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 16, sizeof(std::uint8_t));
        return BooleanEnum::get((val));
    }

    SecurityDefinition &lastRptRequested(const BooleanEnum::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 16, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *SecurityStatusMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t securityStatusId() SBE_NOEXCEPT
    {
        return 965;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityStatusSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool securityStatusInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityStatusEncodingOffset() SBE_NOEXCEPT
    {
        return 17;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityStatusEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD char securityStatusRaw() const SBE_NOEXCEPT
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 17, sizeof(char));
        return (val);
    }

    SBE_NODISCARD SecurityStatusEnum::Value securityStatus() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 17, sizeof(char));
        return SecurityStatusEnum::get((val));
    }

    SecurityDefinition &securityStatus(const SecurityStatusEnum::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 17, &val, sizeof(char));
        return *this;
    }

    SBE_NODISCARD static const char *SecurityTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t securityTypeId() SBE_NOEXCEPT
    {
        return 167;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool securityTypeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 18;
    }

    static SBE_CONSTEXPR char securityTypeNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char securityTypeMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char securityTypeMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t securityTypeEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t securityTypeLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *securityType() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 18;
    }

    SBE_NODISCARD char *securityType() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 18;
    }

    SBE_NODISCARD char securityType(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for securityType [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 18 + (index * 1), sizeof(char));
        return (val);
    }

    SecurityDefinition &securityType(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for securityType [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 18 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSecurityType(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getSecurityType [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 18, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getSecurityTypeAsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 18;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 8);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putSecurityType(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 8, "array too large for putSecurityType");

        std::memcpy(m_buffer + m_offset + 18, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 8; ++start)
        {
            m_buffer[m_offset + 18 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    SecurityDefinition &putSecurityType(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinition &putSecurityType(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 18, src, sizeof(char) * 8);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putSecurityType(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putSecurityType(std::span<const char, N>(src));
    }
    #endif

    SBE_NODISCARD std::string getSecurityTypeAsString() const
    {
        const char *buffer = m_buffer + m_offset + 18;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSecurityTypeAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getSecurityTypeAsString();

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
    SBE_NODISCARD std::string_view getSecurityTypeAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 18;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    SecurityDefinition &putSecurityType(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSecurityType [E106]");
        }

        std::memcpy(m_buffer + m_offset + 18, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 18 + start] = 0;
        }

        return *this;
    }
    #else
    SecurityDefinition &putSecurityType(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putSecurityType [E106]");
        }

        std::memcpy(m_buffer + m_offset + 18, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 18 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *SettlTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t settlTypeId() SBE_NOEXCEPT
    {
        return 63;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t settlTypeSinceVersion() SBE_NOEXCEPT
    {
        return 10;
    }

    SBE_NODISCARD bool settlTypeInActingVersion() SBE_NOEXCEPT
    {
        return m_actingVersion >= settlTypeSinceVersion();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t settlTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 26;
    }

    static SBE_CONSTEXPR char settlTypeNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char settlTypeMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char settlTypeMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t settlTypeEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    static SBE_CONSTEXPR std::uint64_t settlTypeLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD const char *settlType() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 10)
        {
            return nullptr;
        }

        return m_buffer + m_offset + 26;
    }

    SBE_NODISCARD char *settlType() SBE_NOEXCEPT
    {
        if (m_actingVersion < 10)
        {
            return nullptr;
        }

        return m_buffer + m_offset + 26;
    }

    SBE_NODISCARD char settlType(const std::uint64_t index) const
    {
        if (index >= 4)
        {
            throw std::runtime_error("index out of range for settlType [E104]");
        }

        if (m_actingVersion < 10)
        {
            return static_cast<char>(0);
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 26 + (index * 1), sizeof(char));
        return (val);
    }

    SecurityDefinition &settlType(const std::uint64_t index, const char value)
    {
        if (index >= 4)
        {
            throw std::runtime_error("index out of range for settlType [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 26 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSettlType(char *const dst, const std::uint64_t length) const
    {
        if (length > 4)
        {
            throw std::runtime_error("length too large for getSettlType [E106]");
        }

        if (m_actingVersion < 10)
        {
            return 0;
        }

        std::memcpy(dst, m_buffer + m_offset + 26, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getSettlTypeAsSpan() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 10)
        {
            return std::span<const char>();
        }

        const char *buffer = m_buffer + m_offset + 26;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 4);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putSettlType(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 4, "array too large for putSettlType");

        std::memcpy(m_buffer + m_offset + 26, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 4; ++start)
        {
            m_buffer[m_offset + 26 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    SecurityDefinition &putSettlType(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinition &putSettlType(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 26, src, sizeof(char) * 4);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putSettlType(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putSettlType(std::span<const char, N>(src));
    }
    #endif

    SecurityDefinition &putSettlType(
        const char value0,
        const char value1,
        const char value2,
        const char value3) SBE_NOEXCEPT
    {
        char val0 = (value0);
        std::memcpy(m_buffer + m_offset + 26, &val0, sizeof(char));
        char val1 = (value1);
        std::memcpy(m_buffer + m_offset + 27, &val1, sizeof(char));
        char val2 = (value2);
        std::memcpy(m_buffer + m_offset + 28, &val2, sizeof(char));
        char val3 = (value3);
        std::memcpy(m_buffer + m_offset + 29, &val3, sizeof(char));

        return *this;
    }

    SBE_NODISCARD std::string getSettlTypeAsString() const
    {
        if (m_actingVersion < 10)
        {
            return std::string("");
        }

        const char *buffer = m_buffer + m_offset + 26;
        std::size_t length = 0;

        for (; length < 4 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSettlTypeAsJsonEscapedString()
    {
        if (m_actingVersion < 10)
        {
            return std::string("");
        }

        std::ostringstream oss;
        std::string s = getSettlTypeAsString();

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
    SBE_NODISCARD std::string_view getSettlTypeAsStringView() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 10)
        {
            return std::string_view("");
        }

        const char *buffer = m_buffer + m_offset + 26;
        std::size_t length = 0;

        for (; length < 4 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    SecurityDefinition &putSettlType(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 4)
        {
            throw std::runtime_error("string too large for putSettlType [E106]");
        }

        std::memcpy(m_buffer + m_offset + 26, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 4; ++start)
        {
            m_buffer[m_offset + 26 + start] = 0;
        }

        return *this;
    }
    #else
    SecurityDefinition &putSettlType(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 4)
        {
            throw std::runtime_error("string too large for putSettlType [E106]");
        }

        std::memcpy(m_buffer + m_offset + 26, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 4; ++start)
        {
            m_buffer[m_offset + 26 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *SettlDateMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "LocalMktDate";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t settlDateId() SBE_NOEXCEPT
    {
        return 64;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t settlDateSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool settlDateInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t settlDateEncodingOffset() SBE_NOEXCEPT
    {
        return 30;
    }

    static SBE_CONSTEXPR std::uint16_t settlDateNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t settlDateMinValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t settlDateMaxValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t settlDateEncodingLength() SBE_NOEXCEPT
    {
        return 2;
    }

    SBE_NODISCARD std::uint16_t settlDate() const SBE_NOEXCEPT
    {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 30, sizeof(std::uint16_t));
        return SBE_BIG_ENDIAN_ENCODE_16(val);
    }

    SecurityDefinition &settlDate(const std::uint16_t value) SBE_NOEXCEPT
    {
        std::uint16_t val = SBE_BIG_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 30, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *FixingDateMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "LocalMktDate";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t fixingDateId() SBE_NOEXCEPT
    {
        return 30781;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t fixingDateSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool fixingDateInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t fixingDateEncodingOffset() SBE_NOEXCEPT
    {
        return 32;
    }

    static SBE_CONSTEXPR std::uint16_t fixingDateNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t fixingDateMinValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t fixingDateMaxValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t fixingDateEncodingLength() SBE_NOEXCEPT
    {
        return 2;
    }

    SBE_NODISCARD std::uint16_t fixingDate() const SBE_NOEXCEPT
    {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 32, sizeof(std::uint16_t));
        return SBE_BIG_ENDIAN_ENCODE_16(val);
    }

    SecurityDefinition &fixingDate(const std::uint16_t value) SBE_NOEXCEPT
    {
        std::uint16_t val = SBE_BIG_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 32, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *Currency1MetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Currency";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t currency1Id() SBE_NOEXCEPT
    {
        return 30375;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t currency1SinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool currency1InActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t currency1EncodingOffset() SBE_NOEXCEPT
    {
        return 34;
    }

    static SBE_CONSTEXPR char currency1NullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char currency1MinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char currency1MaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t currency1EncodingLength() SBE_NOEXCEPT
    {
        return 3;
    }

    static SBE_CONSTEXPR std::uint64_t currency1Length() SBE_NOEXCEPT
    {
        return 3;
    }

    SBE_NODISCARD const char *currency1() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 34;
    }

    SBE_NODISCARD char *currency1() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 34;
    }

    SBE_NODISCARD char currency1(const std::uint64_t index) const
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for currency1 [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 34 + (index * 1), sizeof(char));
        return (val);
    }

    SecurityDefinition &currency1(const std::uint64_t index, const char value)
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for currency1 [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 34 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getCurrency1(char *const dst, const std::uint64_t length) const
    {
        if (length > 3)
        {
            throw std::runtime_error("length too large for getCurrency1 [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 34, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getCurrency1AsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 34;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 3);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putCurrency1(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 3, "array too large for putCurrency1");

        std::memcpy(m_buffer + m_offset + 34, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 3; ++start)
        {
            m_buffer[m_offset + 34 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    SecurityDefinition &putCurrency1(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinition &putCurrency1(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 34, src, sizeof(char) * 3);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putCurrency1(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putCurrency1(std::span<const char, N>(src));
    }
    #endif

    SecurityDefinition &putCurrency1(
        const char value0,
        const char value1,
        const char value2) SBE_NOEXCEPT
    {
        char val0 = (value0);
        std::memcpy(m_buffer + m_offset + 34, &val0, sizeof(char));
        char val1 = (value1);
        std::memcpy(m_buffer + m_offset + 35, &val1, sizeof(char));
        char val2 = (value2);
        std::memcpy(m_buffer + m_offset + 36, &val2, sizeof(char));

        return *this;
    }

    SBE_NODISCARD std::string getCurrency1AsString() const
    {
        const char *buffer = m_buffer + m_offset + 34;
        std::size_t length = 0;

        for (; length < 3 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getCurrency1AsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getCurrency1AsString();

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
    SBE_NODISCARD std::string_view getCurrency1AsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 34;
        std::size_t length = 0;

        for (; length < 3 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    SecurityDefinition &putCurrency1(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 3)
        {
            throw std::runtime_error("string too large for putCurrency1 [E106]");
        }

        std::memcpy(m_buffer + m_offset + 34, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 3; ++start)
        {
            m_buffer[m_offset + 34 + start] = 0;
        }

        return *this;
    }
    #else
    SecurityDefinition &putCurrency1(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 3)
        {
            throw std::runtime_error("string too large for putCurrency1 [E106]");
        }

        std::memcpy(m_buffer + m_offset + 34, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 3; ++start)
        {
            m_buffer[m_offset + 34 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *Currency2MetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Currency";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t currency2Id() SBE_NOEXCEPT
    {
        return 30376;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t currency2SinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool currency2InActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t currency2EncodingOffset() SBE_NOEXCEPT
    {
        return 37;
    }

    static SBE_CONSTEXPR char currency2NullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char currency2MinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char currency2MaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t currency2EncodingLength() SBE_NOEXCEPT
    {
        return 3;
    }

    static SBE_CONSTEXPR std::uint64_t currency2Length() SBE_NOEXCEPT
    {
        return 3;
    }

    SBE_NODISCARD const char *currency2() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 37;
    }

    SBE_NODISCARD char *currency2() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 37;
    }

    SBE_NODISCARD char currency2(const std::uint64_t index) const
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for currency2 [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 37 + (index * 1), sizeof(char));
        return (val);
    }

    SecurityDefinition &currency2(const std::uint64_t index, const char value)
    {
        if (index >= 3)
        {
            throw std::runtime_error("index out of range for currency2 [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 37 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getCurrency2(char *const dst, const std::uint64_t length) const
    {
        if (length > 3)
        {
            throw std::runtime_error("length too large for getCurrency2 [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 37, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getCurrency2AsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 37;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 3);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putCurrency2(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 3, "array too large for putCurrency2");

        std::memcpy(m_buffer + m_offset + 37, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 3; ++start)
        {
            m_buffer[m_offset + 37 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    SecurityDefinition &putCurrency2(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinition &putCurrency2(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 37, src, sizeof(char) * 3);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putCurrency2(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putCurrency2(std::span<const char, N>(src));
    }
    #endif

    SecurityDefinition &putCurrency2(
        const char value0,
        const char value1,
        const char value2) SBE_NOEXCEPT
    {
        char val0 = (value0);
        std::memcpy(m_buffer + m_offset + 37, &val0, sizeof(char));
        char val1 = (value1);
        std::memcpy(m_buffer + m_offset + 38, &val1, sizeof(char));
        char val2 = (value2);
        std::memcpy(m_buffer + m_offset + 39, &val2, sizeof(char));

        return *this;
    }

    SBE_NODISCARD std::string getCurrency2AsString() const
    {
        const char *buffer = m_buffer + m_offset + 37;
        std::size_t length = 0;

        for (; length < 3 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getCurrency2AsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getCurrency2AsString();

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
    SBE_NODISCARD std::string_view getCurrency2AsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 37;
        std::size_t length = 0;

        for (; length < 3 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    SecurityDefinition &putCurrency2(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 3)
        {
            throw std::runtime_error("string too large for putCurrency2 [E106]");
        }

        std::memcpy(m_buffer + m_offset + 37, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 3; ++start)
        {
            m_buffer[m_offset + 37 + start] = 0;
        }

        return *this;
    }
    #else
    SecurityDefinition &putCurrency2(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 3)
        {
            throw std::runtime_error("string too large for putCurrency2 [E106]");
        }

        std::memcpy(m_buffer + m_offset + 37, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 3; ++start)
        {
            m_buffer[m_offset + 37 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *SymbolMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t symbolId() SBE_NOEXCEPT
    {
        return 55;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t symbolSinceVersion() SBE_NOEXCEPT
    {
        return 3;
    }

    SBE_NODISCARD bool symbolInActingVersion() SBE_NOEXCEPT
    {
        return m_actingVersion >= symbolSinceVersion();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t symbolEncodingOffset() SBE_NOEXCEPT
    {
        return 40;
    }

    static SBE_CONSTEXPR char symbolNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char symbolMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char symbolMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t symbolEncodingLength() SBE_NOEXCEPT
    {
        return 7;
    }

    static SBE_CONSTEXPR std::uint64_t symbolLength() SBE_NOEXCEPT
    {
        return 7;
    }

    SBE_NODISCARD const char *symbol() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 3)
        {
            return nullptr;
        }

        return m_buffer + m_offset + 40;
    }

    SBE_NODISCARD char *symbol() SBE_NOEXCEPT
    {
        if (m_actingVersion < 3)
        {
            return nullptr;
        }

        return m_buffer + m_offset + 40;
    }

    SBE_NODISCARD char symbol(const std::uint64_t index) const
    {
        if (index >= 7)
        {
            throw std::runtime_error("index out of range for symbol [E104]");
        }

        if (m_actingVersion < 3)
        {
            return static_cast<char>(0);
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 40 + (index * 1), sizeof(char));
        return (val);
    }

    SecurityDefinition &symbol(const std::uint64_t index, const char value)
    {
        if (index >= 7)
        {
            throw std::runtime_error("index out of range for symbol [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 40 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getSymbol(char *const dst, const std::uint64_t length) const
    {
        if (length > 7)
        {
            throw std::runtime_error("length too large for getSymbol [E106]");
        }

        if (m_actingVersion < 3)
        {
            return 0;
        }

        std::memcpy(dst, m_buffer + m_offset + 40, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getSymbolAsSpan() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 3)
        {
            return std::span<const char>();
        }

        const char *buffer = m_buffer + m_offset + 40;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 7);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putSymbol(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 7, "array too large for putSymbol");

        std::memcpy(m_buffer + m_offset + 40, src.data(), sizeof(char) * N);
        for (std::size_t start = N; start < 7; ++start)
        {
            m_buffer[m_offset + 40 + start] = 0;
        }

        return *this;
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <typename T>
    SecurityDefinition &putSymbol(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    SecurityDefinition &putSymbol(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 40, src, sizeof(char) * 7);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    SecurityDefinition &putSymbol(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putSymbol(std::span<const char, N>(src));
    }
    #endif

    SBE_NODISCARD std::string getSymbolAsString() const
    {
        if (m_actingVersion < 3)
        {
            return std::string("");
        }

        const char *buffer = m_buffer + m_offset + 40;
        std::size_t length = 0;

        for (; length < 7 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getSymbolAsJsonEscapedString()
    {
        if (m_actingVersion < 3)
        {
            return std::string("");
        }

        std::ostringstream oss;
        std::string s = getSymbolAsString();

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
    SBE_NODISCARD std::string_view getSymbolAsStringView() const SBE_NOEXCEPT
    {
        if (m_actingVersion < 3)
        {
            return std::string_view("");
        }

        const char *buffer = m_buffer + m_offset + 40;
        std::size_t length = 0;

        for (; length < 7 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    SecurityDefinition &putSymbol(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 7)
        {
            throw std::runtime_error("string too large for putSymbol [E106]");
        }

        std::memcpy(m_buffer + m_offset + 40, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 7; ++start)
        {
            m_buffer[m_offset + 40 + start] = 0;
        }

        return *this;
    }
    #else
    SecurityDefinition &putSymbol(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 7)
        {
            throw std::runtime_error("string too large for putSymbol [E106]");
        }

        std::memcpy(m_buffer + m_offset + 40, str.c_str(), srcLength);
        for (std::size_t start = srcLength; start < 7; ++start)
        {
            m_buffer[m_offset + 40 + start] = 0;
        }

        return *this;
    }
    #endif

    SBE_NODISCARD static const char *BookDepthPriceThresholdMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Price";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t bookDepthPriceThresholdId() SBE_NOEXCEPT
    {
        return 31373;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t bookDepthPriceThresholdSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool bookDepthPriceThresholdInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t bookDepthPriceThresholdEncodingOffset() SBE_NOEXCEPT
    {
        return 47;
    }

private:
    ThresholdEncoding m_bookDepthPriceThreshold;

public:
    SBE_NODISCARD ThresholdEncoding &bookDepthPriceThreshold()
    {
        m_bookDepthPriceThreshold.wrap(m_buffer, m_offset + 47, m_actingVersion, m_bufferLength);
        return m_bookDepthPriceThreshold;
    }

    SBE_NODISCARD static const char *DepthOfBookMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t depthOfBookId() SBE_NOEXCEPT
    {
        return 30439;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t depthOfBookSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool depthOfBookInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t depthOfBookEncodingOffset() SBE_NOEXCEPT
    {
        return 55;
    }

    static SBE_CONSTEXPR std::uint32_t depthOfBookNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t depthOfBookMinValue() SBE_NOEXCEPT
    {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t depthOfBookMaxValue() SBE_NOEXCEPT
    {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t depthOfBookEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD std::uint32_t depthOfBook() const SBE_NOEXCEPT
    {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 55, sizeof(std::uint32_t));
        return SBE_BIG_ENDIAN_ENCODE_32(val);
    }

    SecurityDefinition &depthOfBook(const std::uint32_t value) SBE_NOEXCEPT
    {
        std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 55, &val, sizeof(std::uint32_t));
        return *this;
    }

    SBE_NODISCARD static const char *MDPrimaryPortMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t mDPrimaryPortId() SBE_NOEXCEPT
    {
        return 30440;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDPrimaryPortSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool mDPrimaryPortInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDPrimaryPortEncodingOffset() SBE_NOEXCEPT
    {
        return 59;
    }

    static SBE_CONSTEXPR std::uint16_t mDPrimaryPortNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t mDPrimaryPortMinValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t mDPrimaryPortMaxValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t mDPrimaryPortEncodingLength() SBE_NOEXCEPT
    {
        return 2;
    }

    SBE_NODISCARD std::uint16_t mDPrimaryPort() const SBE_NOEXCEPT
    {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 59, sizeof(std::uint16_t));
        return SBE_BIG_ENDIAN_ENCODE_16(val);
    }

    SecurityDefinition &mDPrimaryPort(const std::uint16_t value) SBE_NOEXCEPT
    {
        std::uint16_t val = SBE_BIG_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 59, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *MDSecondaryPortMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t mDSecondaryPortId() SBE_NOEXCEPT
    {
        return 30441;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDSecondaryPortSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool mDSecondaryPortInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDSecondaryPortEncodingOffset() SBE_NOEXCEPT
    {
        return 61;
    }

    static SBE_CONSTEXPR std::uint16_t mDSecondaryPortNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT16;
    }

    static SBE_CONSTEXPR std::uint16_t mDSecondaryPortMinValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(0);
    }

    static SBE_CONSTEXPR std::uint16_t mDSecondaryPortMaxValue() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(65534);
    }

    static SBE_CONSTEXPR std::size_t mDSecondaryPortEncodingLength() SBE_NOEXCEPT
    {
        return 2;
    }

    SBE_NODISCARD std::uint16_t mDSecondaryPort() const SBE_NOEXCEPT
    {
        std::uint16_t val;
        std::memcpy(&val, m_buffer + m_offset + 61, sizeof(std::uint16_t));
        return SBE_BIG_ENDIAN_ENCODE_16(val);
    }

    SecurityDefinition &mDSecondaryPort(const std::uint16_t value) SBE_NOEXCEPT
    {
        std::uint16_t val = SBE_BIG_ENDIAN_ENCODE_16(value);
        std::memcpy(m_buffer + m_offset + 61, &val, sizeof(std::uint16_t));
        return *this;
    }

    SBE_NODISCARD static const char *MinQtyMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t minQtyId() SBE_NOEXCEPT
    {
        return 110;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t minQtySinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool minQtyInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t minQtyEncodingOffset() SBE_NOEXCEPT
    {
        return 63;
    }

private:
    QtyEncoding m_minQty;

public:
    SBE_NODISCARD QtyEncoding &minQty()
    {
        m_minQty.wrap(m_buffer, m_offset + 63, m_actingVersion, m_bufferLength);
        return m_minQty;
    }

    SBE_NODISCARD static const char *LotSizeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t lotSizeId() SBE_NOEXCEPT
    {
        return 1231;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t lotSizeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool lotSizeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t lotSizeEncodingOffset() SBE_NOEXCEPT
    {
        return 71;
    }

private:
    QtyEncoding m_lotSize;

public:
    SBE_NODISCARD QtyEncoding &lotSize()
    {
        m_lotSize.wrap(m_buffer, m_offset + 71, m_actingVersion, m_bufferLength);
        return m_lotSize;
    }

    SBE_NODISCARD static const char *ExDestinationMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t exDestinationId() SBE_NOEXCEPT
    {
        return 100;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t exDestinationSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool exDestinationInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t exDestinationEncodingOffset() SBE_NOEXCEPT
    {
        return 79;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t exDestinationEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t exDestinationRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 79, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD ExDestinationEnum::Value exDestination() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 79, sizeof(std::uint8_t));
        return ExDestinationEnum::get((val));
    }

    SecurityDefinition &exDestination(const ExDestinationEnum::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 79, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *MarketSegmentIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t marketSegmentIDId() SBE_NOEXCEPT
    {
        return 1300;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t marketSegmentIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool marketSegmentIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t marketSegmentIDEncodingOffset() SBE_NOEXCEPT
    {
        return 80;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t marketSegmentIDEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD std::uint8_t marketSegmentIDRaw() const SBE_NOEXCEPT
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 80, sizeof(std::uint8_t));
        return (val);
    }

    SBE_NODISCARD MarketSegmentIDEnum::Value marketSegmentID() const
    {
        std::uint8_t val;
        std::memcpy(&val, m_buffer + m_offset + 80, sizeof(std::uint8_t));
        return MarketSegmentIDEnum::get((val));
    }

    SecurityDefinition &marketSegmentID(const MarketSegmentIDEnum::Value value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 80, &val, sizeof(std::uint8_t));
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
        return 81;
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
        std::memcpy(&val, m_buffer + m_offset + 81, sizeof(std::uint8_t));
        return (val);
    }

    SecurityDefinition &mDSubBookType(const std::uint8_t value) SBE_NOEXCEPT
    {
        std::uint8_t val = (value);
        std::memcpy(m_buffer + m_offset + 81, &val, sizeof(std::uint8_t));
        return *this;
    }

    SBE_NODISCARD static const char *SecurityResponseTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t securityResponseTypeId() SBE_NOEXCEPT
    {
        return 323;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityResponseTypeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool securityResponseTypeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityResponseTypeEncodingOffset() SBE_NOEXCEPT
    {
        return 82;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityResponseTypeEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD char securityResponseTypeRaw() const SBE_NOEXCEPT
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 82, sizeof(char));
        return (val);
    }

    SBE_NODISCARD SecurityResponseTypeEnum::Value securityResponseType() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 82, sizeof(char));
        return SecurityResponseTypeEnum::get((val));
    }

    SecurityDefinition &securityResponseType(const SecurityResponseTypeEnum::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 82, &val, sizeof(char));
        return *this;
    }

    SBE_NODISCARD static const char *SecurityRejectReasonMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t securityRejectReasonId() SBE_NOEXCEPT
    {
        return 1607;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityRejectReasonSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool securityRejectReasonInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityRejectReasonEncodingOffset() SBE_NOEXCEPT
    {
        return 83;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityRejectReasonEncodingLength() SBE_NOEXCEPT
    {
        return 1;
    }

    SBE_NODISCARD char securityRejectReasonRaw() const SBE_NOEXCEPT
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 83, sizeof(char));
        return (val);
    }

    SBE_NODISCARD SecurityRejectReasonEnum::Value securityRejectReason() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 83, sizeof(char));
        return SecurityRejectReasonEnum::get((val));
    }

    SecurityDefinition &securityRejectReason(const SecurityRejectReasonEnum::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 83, &val, sizeof(char));
        return *this;
    }

    SBE_NODISCARD static const char *TimedOrderMinDurationMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t timedOrderMinDurationId() SBE_NOEXCEPT
    {
        return 30793;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t timedOrderMinDurationSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool timedOrderMinDurationInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t timedOrderMinDurationEncodingOffset() SBE_NOEXCEPT
    {
        return 84;
    }

    static SBE_CONSTEXPR std::uint32_t timedOrderMinDurationNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t timedOrderMinDurationMinValue() SBE_NOEXCEPT
    {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t timedOrderMinDurationMaxValue() SBE_NOEXCEPT
    {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t timedOrderMinDurationEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD std::uint32_t timedOrderMinDuration() const SBE_NOEXCEPT
    {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 84, sizeof(std::uint32_t));
        return SBE_BIG_ENDIAN_ENCODE_32(val);
    }

    SecurityDefinition &timedOrderMinDuration(const std::uint32_t value) SBE_NOEXCEPT
    {
        std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 84, &val, sizeof(std::uint32_t));
        return *this;
    }

    SBE_NODISCARD static const char *TimedOrderMaxDurationMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t timedOrderMaxDurationId() SBE_NOEXCEPT
    {
        return 30794;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t timedOrderMaxDurationSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool timedOrderMaxDurationInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t timedOrderMaxDurationEncodingOffset() SBE_NOEXCEPT
    {
        return 88;
    }

    static SBE_CONSTEXPR std::uint32_t timedOrderMaxDurationNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t timedOrderMaxDurationMinValue() SBE_NOEXCEPT
    {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t timedOrderMaxDurationMaxValue() SBE_NOEXCEPT
    {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t timedOrderMaxDurationEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD std::uint32_t timedOrderMaxDuration() const SBE_NOEXCEPT
    {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 88, sizeof(std::uint32_t));
        return SBE_BIG_ENDIAN_ENCODE_32(val);
    }

    SecurityDefinition &timedOrderMaxDuration(const std::uint32_t value) SBE_NOEXCEPT
    {
        std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 88, &val, sizeof(std::uint32_t));
        return *this;
    }

    SBE_NODISCARD static const char *MinimumQuoteLifeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "int";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t minimumQuoteLifeId() SBE_NOEXCEPT
    {
        return 30795;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t minimumQuoteLifeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool minimumQuoteLifeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t minimumQuoteLifeEncodingOffset() SBE_NOEXCEPT
    {
        return 92;
    }

    static SBE_CONSTEXPR std::uint32_t minimumQuoteLifeNullValue() SBE_NOEXCEPT
    {
        return SBE_NULLVALUE_UINT32;
    }

    static SBE_CONSTEXPR std::uint32_t minimumQuoteLifeMinValue() SBE_NOEXCEPT
    {
        return UINT32_C(0x0);
    }

    static SBE_CONSTEXPR std::uint32_t minimumQuoteLifeMaxValue() SBE_NOEXCEPT
    {
        return UINT32_C(0xfffffffe);
    }

    static SBE_CONSTEXPR std::size_t minimumQuoteLifeEncodingLength() SBE_NOEXCEPT
    {
        return 4;
    }

    SBE_NODISCARD std::uint32_t minimumQuoteLife() const SBE_NOEXCEPT
    {
        std::uint32_t val;
        std::memcpy(&val, m_buffer + m_offset + 92, sizeof(std::uint32_t));
        return SBE_BIG_ENDIAN_ENCODE_32(val);
    }

    SecurityDefinition &minimumQuoteLife(const std::uint32_t value) SBE_NOEXCEPT
    {
        std::uint32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
        std::memcpy(m_buffer + m_offset + 92, &val, sizeof(std::uint32_t));
        return *this;
    }

    SBE_NODISCARD static const char *MinIcebergOrderTipSizeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t minIcebergOrderTipSizeId() SBE_NOEXCEPT
    {
        return 30796;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t minIcebergOrderTipSizeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool minIcebergOrderTipSizeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t minIcebergOrderTipSizeEncodingOffset() SBE_NOEXCEPT
    {
        return 96;
    }

private:
    QtyEncoding m_minIcebergOrderTipSize;

public:
    SBE_NODISCARD QtyEncoding &minIcebergOrderTipSize()
    {
        m_minIcebergOrderTipSize.wrap(m_buffer, m_offset + 96, m_actingVersion, m_bufferLength);
        return m_minIcebergOrderTipSize;
    }

    SBE_NODISCARD static const char *MaxIcebergOrderTipSizeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "Qty";
            case MetaAttribute::PRESENCE: return "optional";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t maxIcebergOrderTipSizeId() SBE_NOEXCEPT
    {
        return 30797;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t maxIcebergOrderTipSizeSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool maxIcebergOrderTipSizeInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t maxIcebergOrderTipSizeEncodingOffset() SBE_NOEXCEPT
    {
        return 104;
    }

private:
    QtyEncoding m_maxIcebergOrderTipSize;

public:
    SBE_NODISCARD QtyEncoding &maxIcebergOrderTipSize()
    {
        m_maxIcebergOrderTipSize.wrap(m_buffer, m_offset + 104, m_actingVersion, m_bufferLength);
        return m_maxIcebergOrderTipSize;
    }

    class RatePrecisionGrp
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
        RatePrecisionGrp() = default;

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
            dimensions.blockLength(static_cast<std::uint16_t>(25));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 25;
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
            return 25;
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

        inline RatePrecisionGrp &next()
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


        SBE_NODISCARD static const char *MinimumValueMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t minimumValueId() SBE_NOEXCEPT
        {
            return 30778;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t minimumValueSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool minimumValueInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t minimumValueEncodingOffset() SBE_NOEXCEPT
        {
            return 0;
        }

private:
        DecimalEncoding m_minimumValue;

public:
        SBE_NODISCARD DecimalEncoding &minimumValue()
        {
            m_minimumValue.wrap(m_buffer, m_offset + 0, m_actingVersion, m_bufferLength);
            return m_minimumValue;
        }

        SBE_NODISCARD static const char *MaximumValueMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "Price";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t maximumValueId() SBE_NOEXCEPT
        {
            return 30779;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t maximumValueSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool maximumValueInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t maximumValueEncodingOffset() SBE_NOEXCEPT
        {
            return 8;
        }

private:
        DecimalEncoding m_maximumValue;

public:
        SBE_NODISCARD DecimalEncoding &maximumValue()
        {
            m_maximumValue.wrap(m_buffer, m_offset + 8, m_actingVersion, m_bufferLength);
            return m_maximumValue;
        }

        SBE_NODISCARD static const char *RatePrecisionMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t ratePrecisionId() SBE_NOEXCEPT
        {
            return 30379;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t ratePrecisionSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool ratePrecisionInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t ratePrecisionEncodingOffset() SBE_NOEXCEPT
        {
            return 16;
        }

        static SBE_CONSTEXPR std::int64_t ratePrecisionNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_INT64;
        }

        static SBE_CONSTEXPR std::int64_t ratePrecisionMinValue() SBE_NOEXCEPT
        {
            return INT64_C(-9223372036854775807);
        }

        static SBE_CONSTEXPR std::int64_t ratePrecisionMaxValue() SBE_NOEXCEPT
        {
            return INT64_C(9223372036854775807);
        }

        static SBE_CONSTEXPR std::size_t ratePrecisionEncodingLength() SBE_NOEXCEPT
        {
            return 8;
        }

        SBE_NODISCARD std::int64_t ratePrecision() const SBE_NOEXCEPT
        {
            std::int64_t val;
            std::memcpy(&val, m_buffer + m_offset + 16, sizeof(std::int64_t));
            return SBE_BIG_ENDIAN_ENCODE_64(val);
        }

        RatePrecisionGrp &ratePrecision(const std::int64_t value) SBE_NOEXCEPT
        {
            std::int64_t val = SBE_BIG_ENDIAN_ENCODE_64(value);
            std::memcpy(m_buffer + m_offset + 16, &val, sizeof(std::int64_t));
            return *this;
        }

        SBE_NODISCARD static const char *RightDpsMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t rightDpsId() SBE_NOEXCEPT
        {
            return 30435;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t rightDpsSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool rightDpsInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t rightDpsEncodingOffset() SBE_NOEXCEPT
        {
            return 24;
        }

        static SBE_CONSTEXPR std::uint8_t rightDpsNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_UINT8;
        }

        static SBE_CONSTEXPR std::uint8_t rightDpsMinValue() SBE_NOEXCEPT
        {
            return static_cast<std::uint8_t>(0);
        }

        static SBE_CONSTEXPR std::uint8_t rightDpsMaxValue() SBE_NOEXCEPT
        {
            return static_cast<std::uint8_t>(254);
        }

        static SBE_CONSTEXPR std::size_t rightDpsEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD std::uint8_t rightDps() const SBE_NOEXCEPT
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 24, sizeof(std::uint8_t));
            return (val);
        }

        RatePrecisionGrp &rightDps(const std::uint8_t value) SBE_NOEXCEPT
        {
            std::uint8_t val = (value);
            std::memcpy(m_buffer + m_offset + 24, &val, sizeof(std::uint8_t));
            return *this;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, RatePrecisionGrp &writer)
        {
            builder << '{';
            builder << R"("MinimumValue": )";
            builder << writer.minimumValue();

            builder << ", ";
            builder << R"("MaximumValue": )";
            builder << writer.maximumValue();

            builder << ", ";
            builder << R"("RatePrecision": )";
            builder << +writer.ratePrecision();

            builder << ", ";
            builder << R"("RightDps": )";
            builder << +writer.rightDps();

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
    RatePrecisionGrp m_ratePrecisionGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t RatePrecisionGrpId() SBE_NOEXCEPT
    {
        return 30777;
    }

    SBE_NODISCARD inline RatePrecisionGrp &ratePrecisionGrp()
    {
        m_ratePrecisionGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_ratePrecisionGrp;
    }

    RatePrecisionGrp &ratePrecisionGrpCount(const std::uint16_t count)
    {
        m_ratePrecisionGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_ratePrecisionGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t ratePrecisionGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool ratePrecisionGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

    class SecAltIDGrp
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
        SecAltIDGrp() = default;

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
            dimensions.blockLength(static_cast<std::uint16_t>(13));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 13;
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
            return 13;
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

        inline SecAltIDGrp &next()
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


        SBE_NODISCARD static const char *SecurityAltIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t securityAltIDId() SBE_NOEXCEPT
        {
            return 455;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityAltIDSinceVersion() SBE_NOEXCEPT
        {
            return 2;
        }

        SBE_NODISCARD bool securityAltIDInActingVersion() SBE_NOEXCEPT
        {
            return m_actingVersion >= securityAltIDSinceVersion();
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityAltIDEncodingOffset() SBE_NOEXCEPT
        {
            return 0;
        }

        static SBE_CONSTEXPR char securityAltIDNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char securityAltIDMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char securityAltIDMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t securityAltIDEncodingLength() SBE_NOEXCEPT
        {
            return 12;
        }

        static SBE_CONSTEXPR std::uint64_t securityAltIDLength() SBE_NOEXCEPT
        {
            return 12;
        }

        SBE_NODISCARD const char *securityAltID() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return nullptr;
            }

            return m_buffer + m_offset + 0;
        }

        SBE_NODISCARD char *securityAltID() SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return nullptr;
            }

            return m_buffer + m_offset + 0;
        }

        SBE_NODISCARD char securityAltID(const std::uint64_t index) const
        {
            if (index >= 12)
            {
                throw std::runtime_error("index out of range for securityAltID [E104]");
            }

            if (m_actingVersion < 2)
            {
                return static_cast<char>(0);
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
            return (val);
        }

        SecAltIDGrp &securityAltID(const std::uint64_t index, const char value)
        {
            if (index >= 12)
            {
                throw std::runtime_error("index out of range for securityAltID [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getSecurityAltID(char *const dst, const std::uint64_t length) const
        {
            if (length > 12)
            {
                throw std::runtime_error("length too large for getSecurityAltID [E106]");
            }

            if (m_actingVersion < 2)
            {
                return 0;
            }

            std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getSecurityAltIDAsSpan() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return std::span<const char>();
            }

            const char *buffer = m_buffer + m_offset + 0;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 12);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        SecAltIDGrp &putSecurityAltID(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 12, "array too large for putSecurityAltID");

            std::memcpy(m_buffer + m_offset + 0, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 12; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        SecAltIDGrp &putSecurityAltID(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        SecAltIDGrp &putSecurityAltID(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 12);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        SecAltIDGrp &putSecurityAltID(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putSecurityAltID(std::span<const char, N>(src));
        }
        #endif

        SBE_NODISCARD std::string getSecurityAltIDAsString() const
        {
            if (m_actingVersion < 2)
            {
                return std::string("");
            }

            const char *buffer = m_buffer + m_offset + 0;
            std::size_t length = 0;

            for (; length < 12 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getSecurityAltIDAsJsonEscapedString()
        {
            if (m_actingVersion < 2)
            {
                return std::string("");
            }

            std::ostringstream oss;
            std::string s = getSecurityAltIDAsString();

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
        SBE_NODISCARD std::string_view getSecurityAltIDAsStringView() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return std::string_view("");
            }

            const char *buffer = m_buffer + m_offset + 0;
            std::size_t length = 0;

            for (; length < 12 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        SecAltIDGrp &putSecurityAltID(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 12)
            {
                throw std::runtime_error("string too large for putSecurityAltID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 12; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #else
        SecAltIDGrp &putSecurityAltID(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 12)
            {
                throw std::runtime_error("string too large for putSecurityAltID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 12; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *SecurityAltIDSourceMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t securityAltIDSourceId() SBE_NOEXCEPT
        {
            return 456;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t securityAltIDSourceSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool securityAltIDSourceInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityAltIDSourceEncodingOffset() SBE_NOEXCEPT
        {
            return 12;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t securityAltIDSourceEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD char securityAltIDSourceRaw() const SBE_NOEXCEPT
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 12, sizeof(char));
            return (val);
        }

        SBE_NODISCARD SecurityAltIDSourceEnum::Value securityAltIDSource() const
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 12, sizeof(char));
            return SecurityAltIDSourceEnum::get((val));
        }

        SecAltIDGrp &securityAltIDSource(const SecurityAltIDSourceEnum::Value value) SBE_NOEXCEPT
        {
            char val = (value);
            std::memcpy(m_buffer + m_offset + 12, &val, sizeof(char));
            return *this;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, SecAltIDGrp &writer)
        {
            builder << '{';
            builder << R"("SecurityAltID": )";
            builder << '"' <<
                writer.getSecurityAltIDAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("SecurityAltIDSource": )";
            builder << '"' << writer.securityAltIDSource() << '"';

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
    SecAltIDGrp m_secAltIDGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t SecAltIDGrpId() SBE_NOEXCEPT
    {
        return 35003;
    }

    SBE_NODISCARD inline SecAltIDGrp &secAltIDGrp()
    {
        m_secAltIDGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_secAltIDGrp;
    }

    SecAltIDGrp &secAltIDGrpCount(const std::uint16_t count)
    {
        m_secAltIDGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_secAltIDGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t secAltIDGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool secAltIDGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

    class LegsGrp
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
        LegsGrp() = default;

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
            dimensions.blockLength(static_cast<std::uint16_t>(20));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 20;
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
            return 20;
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

        inline LegsGrp &next()
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


        SBE_NODISCARD static const char *LegIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t legIDId() SBE_NOEXCEPT
        {
            return 1788;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t legIDSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool legIDInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legIDEncodingOffset() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legIDEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD std::uint8_t legIDRaw() const SBE_NOEXCEPT
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 0, sizeof(std::uint8_t));
            return (val);
        }

        SBE_NODISCARD LegIDEnum::Value legID() const
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 0, sizeof(std::uint8_t));
            return LegIDEnum::get((val));
        }

        LegsGrp &legID(const LegIDEnum::Value value) SBE_NOEXCEPT
        {
            std::uint8_t val = (value);
            std::memcpy(m_buffer + m_offset + 0, &val, sizeof(std::uint8_t));
            return *this;
        }

        SBE_NODISCARD static const char *LegSettlTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t legSettlTypeId() SBE_NOEXCEPT
        {
            return 587;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t legSettlTypeSinceVersion() SBE_NOEXCEPT
        {
            return 10;
        }

        SBE_NODISCARD bool legSettlTypeInActingVersion() SBE_NOEXCEPT
        {
            return m_actingVersion >= legSettlTypeSinceVersion();
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legSettlTypeEncodingOffset() SBE_NOEXCEPT
        {
            return 1;
        }

        static SBE_CONSTEXPR char legSettlTypeNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char legSettlTypeMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char legSettlTypeMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t legSettlTypeEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        static SBE_CONSTEXPR std::uint64_t legSettlTypeLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD const char *legSettlType() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 10)
            {
                return nullptr;
            }

            return m_buffer + m_offset + 1;
        }

        SBE_NODISCARD char *legSettlType() SBE_NOEXCEPT
        {
            if (m_actingVersion < 10)
            {
                return nullptr;
            }

            return m_buffer + m_offset + 1;
        }

        SBE_NODISCARD char legSettlType(const std::uint64_t index) const
        {
            if (index >= 4)
            {
                throw std::runtime_error("index out of range for legSettlType [E104]");
            }

            if (m_actingVersion < 10)
            {
                return static_cast<char>(0);
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 1 + (index * 1), sizeof(char));
            return (val);
        }

        LegsGrp &legSettlType(const std::uint64_t index, const char value)
        {
            if (index >= 4)
            {
                throw std::runtime_error("index out of range for legSettlType [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 1 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getLegSettlType(char *const dst, const std::uint64_t length) const
        {
            if (length > 4)
            {
                throw std::runtime_error("length too large for getLegSettlType [E106]");
            }

            if (m_actingVersion < 10)
            {
                return 0;
            }

            std::memcpy(dst, m_buffer + m_offset + 1, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getLegSettlTypeAsSpan() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 10)
            {
                return std::span<const char>();
            }

            const char *buffer = m_buffer + m_offset + 1;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 4);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        LegsGrp &putLegSettlType(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 4, "array too large for putLegSettlType");

            std::memcpy(m_buffer + m_offset + 1, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 4; ++start)
            {
                m_buffer[m_offset + 1 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        LegsGrp &putLegSettlType(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        LegsGrp &putLegSettlType(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 1, src, sizeof(char) * 4);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        LegsGrp &putLegSettlType(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putLegSettlType(std::span<const char, N>(src));
        }
        #endif

        LegsGrp &putLegSettlType(
            const char value0,
            const char value1,
            const char value2,
            const char value3) SBE_NOEXCEPT
        {
            char val0 = (value0);
            std::memcpy(m_buffer + m_offset + 1, &val0, sizeof(char));
            char val1 = (value1);
            std::memcpy(m_buffer + m_offset + 2, &val1, sizeof(char));
            char val2 = (value2);
            std::memcpy(m_buffer + m_offset + 3, &val2, sizeof(char));
            char val3 = (value3);
            std::memcpy(m_buffer + m_offset + 4, &val3, sizeof(char));

            return *this;
        }

        SBE_NODISCARD std::string getLegSettlTypeAsString() const
        {
            if (m_actingVersion < 10)
            {
                return std::string("");
            }

            const char *buffer = m_buffer + m_offset + 1;
            std::size_t length = 0;

            for (; length < 4 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getLegSettlTypeAsJsonEscapedString()
        {
            if (m_actingVersion < 10)
            {
                return std::string("");
            }

            std::ostringstream oss;
            std::string s = getLegSettlTypeAsString();

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
        SBE_NODISCARD std::string_view getLegSettlTypeAsStringView() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 10)
            {
                return std::string_view("");
            }

            const char *buffer = m_buffer + m_offset + 1;
            std::size_t length = 0;

            for (; length < 4 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        LegsGrp &putLegSettlType(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 4)
            {
                throw std::runtime_error("string too large for putLegSettlType [E106]");
            }

            std::memcpy(m_buffer + m_offset + 1, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 4; ++start)
            {
                m_buffer[m_offset + 1 + start] = 0;
            }

            return *this;
        }
        #else
        LegsGrp &putLegSettlType(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 4)
            {
                throw std::runtime_error("string too large for putLegSettlType [E106]");
            }

            std::memcpy(m_buffer + m_offset + 1, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 4; ++start)
            {
                m_buffer[m_offset + 1 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *LegSettlDateMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "LocalMktDate";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t legSettlDateId() SBE_NOEXCEPT
        {
            return 588;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t legSettlDateSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool legSettlDateInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legSettlDateEncodingOffset() SBE_NOEXCEPT
        {
            return 5;
        }

        static SBE_CONSTEXPR std::uint16_t legSettlDateNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_UINT16;
        }

        static SBE_CONSTEXPR std::uint16_t legSettlDateMinValue() SBE_NOEXCEPT
        {
            return static_cast<std::uint16_t>(0);
        }

        static SBE_CONSTEXPR std::uint16_t legSettlDateMaxValue() SBE_NOEXCEPT
        {
            return static_cast<std::uint16_t>(65534);
        }

        static SBE_CONSTEXPR std::size_t legSettlDateEncodingLength() SBE_NOEXCEPT
        {
            return 2;
        }

        SBE_NODISCARD std::uint16_t legSettlDate() const SBE_NOEXCEPT
        {
            std::uint16_t val;
            std::memcpy(&val, m_buffer + m_offset + 5, sizeof(std::uint16_t));
            return SBE_BIG_ENDIAN_ENCODE_16(val);
        }

        LegsGrp &legSettlDate(const std::uint16_t value) SBE_NOEXCEPT
        {
            std::uint16_t val = SBE_BIG_ENDIAN_ENCODE_16(value);
            std::memcpy(m_buffer + m_offset + 5, &val, sizeof(std::uint16_t));
            return *this;
        }

        SBE_NODISCARD static const char *LegSecurityIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t legSecurityIDId() SBE_NOEXCEPT
        {
            return 602;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t legSecurityIDSinceVersion() SBE_NOEXCEPT
        {
            return 2;
        }

        SBE_NODISCARD bool legSecurityIDInActingVersion() SBE_NOEXCEPT
        {
            return m_actingVersion >= legSecurityIDSinceVersion();
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legSecurityIDEncodingOffset() SBE_NOEXCEPT
        {
            return 7;
        }

        static SBE_CONSTEXPR char legSecurityIDNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char legSecurityIDMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char legSecurityIDMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t legSecurityIDEncodingLength() SBE_NOEXCEPT
        {
            return 12;
        }

        static SBE_CONSTEXPR std::uint64_t legSecurityIDLength() SBE_NOEXCEPT
        {
            return 12;
        }

        SBE_NODISCARD const char *legSecurityID() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return nullptr;
            }

            return m_buffer + m_offset + 7;
        }

        SBE_NODISCARD char *legSecurityID() SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return nullptr;
            }

            return m_buffer + m_offset + 7;
        }

        SBE_NODISCARD char legSecurityID(const std::uint64_t index) const
        {
            if (index >= 12)
            {
                throw std::runtime_error("index out of range for legSecurityID [E104]");
            }

            if (m_actingVersion < 2)
            {
                return static_cast<char>(0);
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 7 + (index * 1), sizeof(char));
            return (val);
        }

        LegsGrp &legSecurityID(const std::uint64_t index, const char value)
        {
            if (index >= 12)
            {
                throw std::runtime_error("index out of range for legSecurityID [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 7 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getLegSecurityID(char *const dst, const std::uint64_t length) const
        {
            if (length > 12)
            {
                throw std::runtime_error("length too large for getLegSecurityID [E106]");
            }

            if (m_actingVersion < 2)
            {
                return 0;
            }

            std::memcpy(dst, m_buffer + m_offset + 7, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getLegSecurityIDAsSpan() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return std::span<const char>();
            }

            const char *buffer = m_buffer + m_offset + 7;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 12);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        LegsGrp &putLegSecurityID(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 12, "array too large for putLegSecurityID");

            std::memcpy(m_buffer + m_offset + 7, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 12; ++start)
            {
                m_buffer[m_offset + 7 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        LegsGrp &putLegSecurityID(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        LegsGrp &putLegSecurityID(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 7, src, sizeof(char) * 12);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        LegsGrp &putLegSecurityID(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putLegSecurityID(std::span<const char, N>(src));
        }
        #endif

        SBE_NODISCARD std::string getLegSecurityIDAsString() const
        {
            if (m_actingVersion < 2)
            {
                return std::string("");
            }

            const char *buffer = m_buffer + m_offset + 7;
            std::size_t length = 0;

            for (; length < 12 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getLegSecurityIDAsJsonEscapedString()
        {
            if (m_actingVersion < 2)
            {
                return std::string("");
            }

            std::ostringstream oss;
            std::string s = getLegSecurityIDAsString();

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
        SBE_NODISCARD std::string_view getLegSecurityIDAsStringView() const SBE_NOEXCEPT
        {
            if (m_actingVersion < 2)
            {
                return std::string_view("");
            }

            const char *buffer = m_buffer + m_offset + 7;
            std::size_t length = 0;

            for (; length < 12 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        LegsGrp &putLegSecurityID(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 12)
            {
                throw std::runtime_error("string too large for putLegSecurityID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 7, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 12; ++start)
            {
                m_buffer[m_offset + 7 + start] = 0;
            }

            return *this;
        }
        #else
        LegsGrp &putLegSecurityID(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 12)
            {
                throw std::runtime_error("string too large for putLegSecurityID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 7, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 12; ++start)
            {
                m_buffer[m_offset + 7 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *LegSecurityIDSourceMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t legSecurityIDSourceId() SBE_NOEXCEPT
        {
            return 603;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t legSecurityIDSourceSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool legSecurityIDSourceInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legSecurityIDSourceEncodingOffset() SBE_NOEXCEPT
        {
            return 19;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t legSecurityIDSourceEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD char legSecurityIDSourceRaw() const SBE_NOEXCEPT
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 19, sizeof(char));
            return (val);
        }

        SBE_NODISCARD LegSecurityIDSourceEnum::Value legSecurityIDSource() const
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 19, sizeof(char));
            return LegSecurityIDSourceEnum::get((val));
        }

        LegsGrp &legSecurityIDSource(const LegSecurityIDSourceEnum::Value value) SBE_NOEXCEPT
        {
            char val = (value);
            std::memcpy(m_buffer + m_offset + 19, &val, sizeof(char));
            return *this;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, LegsGrp &writer)
        {
            builder << '{';
            builder << R"("LegID": )";
            builder << '"' << writer.legID() << '"';

            builder << ", ";
            builder << R"("LegSettlType": )";
            builder << '"' <<
                writer.getLegSettlTypeAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("LegSettlDate": )";
            builder << +writer.legSettlDate();

            builder << ", ";
            builder << R"("LegSecurityID": )";
            builder << '"' <<
                writer.getLegSecurityIDAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("LegSecurityIDSource": )";
            builder << '"' << writer.legSecurityIDSource() << '"';

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
    LegsGrp m_legsGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t LegsGrpId() SBE_NOEXCEPT
    {
        return 30780;
    }

    SBE_NODISCARD inline LegsGrp &legsGrp()
    {
        m_legsGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_legsGrp;
    }

    LegsGrp &legsGrpCount(const std::uint16_t count)
    {
        m_legsGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_legsGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t legsGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool legsGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

    class IcebergTipDelayGrp
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
        IcebergTipDelayGrp() = default;

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
            dimensions.blockLength(static_cast<std::uint16_t>(18));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 18;
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
            return 18;
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

        inline IcebergTipDelayGrp &next()
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


        SBE_NODISCARD static const char *IcebergTipDelayIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t icebergTipDelayIDId() SBE_NOEXCEPT
        {
            return 30799;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t icebergTipDelayIDSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool icebergTipDelayIDInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t icebergTipDelayIDEncodingOffset() SBE_NOEXCEPT
        {
            return 0;
        }

        static SBE_CONSTEXPR char icebergTipDelayIDNullValue() SBE_NOEXCEPT
        {
            return static_cast<char>(0);
        }

        static SBE_CONSTEXPR char icebergTipDelayIDMinValue() SBE_NOEXCEPT
        {
            return static_cast<char>(32);
        }

        static SBE_CONSTEXPR char icebergTipDelayIDMaxValue() SBE_NOEXCEPT
        {
            return static_cast<char>(126);
        }

        static SBE_CONSTEXPR std::size_t icebergTipDelayIDEncodingLength() SBE_NOEXCEPT
        {
            return 10;
        }

        static SBE_CONSTEXPR std::uint64_t icebergTipDelayIDLength() SBE_NOEXCEPT
        {
            return 10;
        }

        SBE_NODISCARD const char *icebergTipDelayID() const SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 0;
        }

        SBE_NODISCARD char *icebergTipDelayID() SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 0;
        }

        SBE_NODISCARD char icebergTipDelayID(const std::uint64_t index) const
        {
            if (index >= 10)
            {
                throw std::runtime_error("index out of range for icebergTipDelayID [E104]");
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
            return (val);
        }

        IcebergTipDelayGrp &icebergTipDelayID(const std::uint64_t index, const char value)
        {
            if (index >= 10)
            {
                throw std::runtime_error("index out of range for icebergTipDelayID [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getIcebergTipDelayID(char *const dst, const std::uint64_t length) const
        {
            if (length > 10)
            {
                throw std::runtime_error("length too large for getIcebergTipDelayID [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getIcebergTipDelayIDAsSpan() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 0;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 10);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        IcebergTipDelayGrp &putIcebergTipDelayID(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 10, "array too large for putIcebergTipDelayID");

            std::memcpy(m_buffer + m_offset + 0, src.data(), sizeof(char) * N);
            for (std::size_t start = N; start < 10; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <typename T>
        IcebergTipDelayGrp &putIcebergTipDelayID(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        IcebergTipDelayGrp &putIcebergTipDelayID(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 10);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        IcebergTipDelayGrp &putIcebergTipDelayID(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putIcebergTipDelayID(std::span<const char, N>(src));
        }
        #endif

        SBE_NODISCARD std::string getIcebergTipDelayIDAsString() const
        {
            const char *buffer = m_buffer + m_offset + 0;
            std::size_t length = 0;

            for (; length < 10 && *(buffer + length) != '\0'; ++length);
            std::string result(buffer, length);

            return result;
        }

        std::string getIcebergTipDelayIDAsJsonEscapedString()
        {
            std::ostringstream oss;
            std::string s = getIcebergTipDelayIDAsString();

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
        SBE_NODISCARD std::string_view getIcebergTipDelayIDAsStringView() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 0;
            std::size_t length = 0;

            for (; length < 10 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        IcebergTipDelayGrp &putIcebergTipDelayID(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 10)
            {
                throw std::runtime_error("string too large for putIcebergTipDelayID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 10; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #else
        IcebergTipDelayGrp &putIcebergTipDelayID(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 10)
            {
                throw std::runtime_error("string too large for putIcebergTipDelayID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 10; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *MinReplenishDelayMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t minReplenishDelayId() SBE_NOEXCEPT
        {
            return 30800;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t minReplenishDelaySinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool minReplenishDelayInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t minReplenishDelayEncodingOffset() SBE_NOEXCEPT
        {
            return 10;
        }

        static SBE_CONSTEXPR std::int32_t minReplenishDelayNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_INT32;
        }

        static SBE_CONSTEXPR std::int32_t minReplenishDelayMinValue() SBE_NOEXCEPT
        {
            return INT32_C(-2147483647);
        }

        static SBE_CONSTEXPR std::int32_t minReplenishDelayMaxValue() SBE_NOEXCEPT
        {
            return INT32_C(2147483647);
        }

        static SBE_CONSTEXPR std::size_t minReplenishDelayEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD std::int32_t minReplenishDelay() const SBE_NOEXCEPT
        {
            std::int32_t val;
            std::memcpy(&val, m_buffer + m_offset + 10, sizeof(std::int32_t));
            return SBE_BIG_ENDIAN_ENCODE_32(val);
        }

        IcebergTipDelayGrp &minReplenishDelay(const std::int32_t value) SBE_NOEXCEPT
        {
            std::int32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
            std::memcpy(m_buffer + m_offset + 10, &val, sizeof(std::int32_t));
            return *this;
        }

        SBE_NODISCARD static const char *MaxReplenishDelayMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t maxReplenishDelayId() SBE_NOEXCEPT
        {
            return 30801;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t maxReplenishDelaySinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool maxReplenishDelayInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t maxReplenishDelayEncodingOffset() SBE_NOEXCEPT
        {
            return 14;
        }

        static SBE_CONSTEXPR std::int32_t maxReplenishDelayNullValue() SBE_NOEXCEPT
        {
            return SBE_NULLVALUE_INT32;
        }

        static SBE_CONSTEXPR std::int32_t maxReplenishDelayMinValue() SBE_NOEXCEPT
        {
            return INT32_C(-2147483647);
        }

        static SBE_CONSTEXPR std::int32_t maxReplenishDelayMaxValue() SBE_NOEXCEPT
        {
            return INT32_C(2147483647);
        }

        static SBE_CONSTEXPR std::size_t maxReplenishDelayEncodingLength() SBE_NOEXCEPT
        {
            return 4;
        }

        SBE_NODISCARD std::int32_t maxReplenishDelay() const SBE_NOEXCEPT
        {
            std::int32_t val;
            std::memcpy(&val, m_buffer + m_offset + 14, sizeof(std::int32_t));
            return SBE_BIG_ENDIAN_ENCODE_32(val);
        }

        IcebergTipDelayGrp &maxReplenishDelay(const std::int32_t value) SBE_NOEXCEPT
        {
            std::int32_t val = SBE_BIG_ENDIAN_ENCODE_32(value);
            std::memcpy(m_buffer + m_offset + 14, &val, sizeof(std::int32_t));
            return *this;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, IcebergTipDelayGrp &writer)
        {
            builder << '{';
            builder << R"("IcebergTipDelayID": )";
            builder << '"' <<
                writer.getIcebergTipDelayIDAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("MinReplenishDelay": )";
            builder << +writer.minReplenishDelay();

            builder << ", ";
            builder << R"("MaxReplenishDelay": )";
            builder << +writer.maxReplenishDelay();

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
    IcebergTipDelayGrp m_icebergTipDelayGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t IcebergTipDelayGrpId() SBE_NOEXCEPT
    {
        return 30798;
    }

    SBE_NODISCARD inline IcebergTipDelayGrp &icebergTipDelayGrp()
    {
        m_icebergTipDelayGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_icebergTipDelayGrp;
    }

    IcebergTipDelayGrp &icebergTipDelayGrpCount(const std::uint16_t count)
    {
        m_icebergTipDelayGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_icebergTipDelayGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t icebergTipDelayGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool icebergTipDelayGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, const SecurityDefinition &_writer)
{
    SecurityDefinition writer(
        _writer.m_buffer,
        _writer.m_offset,
        _writer.m_bufferLength,
        _writer.m_actingBlockLength,
        _writer.m_actingVersion);

    builder << '{';
    builder << R"("Name": "SecurityDefinition", )";
    builder << R"("sbeTemplateId": )";
    builder << writer.sbeTemplateId();
    builder << ", ";

    builder << R"("ApplID": )";
    builder << '"' <<
        writer.getApplIDAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("ApplSeqNo": )";
    builder << +writer.applSeqNo();

    builder << ", ";
    builder << R"("LastRptRequested": )";
    builder << '"' << writer.lastRptRequested() << '"';

    builder << ", ";
    builder << R"("SecurityStatus": )";
    builder << '"' << writer.securityStatus() << '"';

    builder << ", ";
    builder << R"("SecurityType": )";
    builder << '"' <<
        writer.getSecurityTypeAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("SettlType": )";
    builder << '"' <<
        writer.getSettlTypeAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("SettlDate": )";
    builder << +writer.settlDate();

    builder << ", ";
    builder << R"("FixingDate": )";
    builder << +writer.fixingDate();

    builder << ", ";
    builder << R"("Currency1": )";
    builder << '"' <<
        writer.getCurrency1AsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("Currency2": )";
    builder << '"' <<
        writer.getCurrency2AsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("Symbol": )";
    builder << '"' <<
        writer.getSymbolAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("BookDepthPriceThreshold": )";
    builder << writer.bookDepthPriceThreshold();

    builder << ", ";
    builder << R"("DepthOfBook": )";
    builder << +writer.depthOfBook();

    builder << ", ";
    builder << R"("MDPrimaryPort": )";
    builder << +writer.mDPrimaryPort();

    builder << ", ";
    builder << R"("MDSecondaryPort": )";
    builder << +writer.mDSecondaryPort();

    builder << ", ";
    builder << R"("MinQty": )";
    builder << writer.minQty();

    builder << ", ";
    builder << R"("LotSize": )";
    builder << writer.lotSize();

    builder << ", ";
    builder << R"("ExDestination": )";
    builder << '"' << writer.exDestination() << '"';

    builder << ", ";
    builder << R"("MarketSegmentID": )";
    builder << '"' << writer.marketSegmentID() << '"';

    builder << ", ";
    builder << R"("MDSubBookType": )";
    builder << +writer.mDSubBookType();

    builder << ", ";
    builder << R"("SecurityResponseType": )";
    builder << '"' << writer.securityResponseType() << '"';

    builder << ", ";
    builder << R"("SecurityRejectReason": )";
    builder << '"' << writer.securityRejectReason() << '"';

    builder << ", ";
    builder << R"("TimedOrderMinDuration": )";
    builder << +writer.timedOrderMinDuration();

    builder << ", ";
    builder << R"("TimedOrderMaxDuration": )";
    builder << +writer.timedOrderMaxDuration();

    builder << ", ";
    builder << R"("MinimumQuoteLife": )";
    builder << +writer.minimumQuoteLife();

    builder << ", ";
    builder << R"("MinIcebergOrderTipSize": )";
    builder << writer.minIcebergOrderTipSize();

    builder << ", ";
    builder << R"("MaxIcebergOrderTipSize": )";
    builder << writer.maxIcebergOrderTipSize();

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("RatePrecisionGrp": [)";
        writer.ratePrecisionGrp().forEach(
            [&](RatePrecisionGrp &ratePrecisionGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << ratePrecisionGrp;
            });
        builder << ']';
    }

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("SecAltIDGrp": [)";
        writer.secAltIDGrp().forEach(
            [&](SecAltIDGrp &secAltIDGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << secAltIDGrp;
            });
        builder << ']';
    }

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("LegsGrp": [)";
        writer.legsGrp().forEach(
            [&](LegsGrp &legsGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << legsGrp;
            });
        builder << ']';
    }

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("IcebergTipDelayGrp": [)";
        writer.icebergTipDelayGrp().forEach(
            [&](IcebergTipDelayGrp &icebergTipDelayGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << icebergTipDelayGrp;
            });
        builder << ']';
    }

    builder << '}';

    return builder;
}

void skip()
{
    auto &ratePrecisionGrpGroup { ratePrecisionGrp() };
    while (ratePrecisionGrpGroup.hasNext())
    {
        ratePrecisionGrpGroup.next().skip();
    }
    auto &secAltIDGrpGroup { secAltIDGrp() };
    while (secAltIDGrpGroup.hasNext())
    {
        secAltIDGrpGroup.next().skip();
    }
    auto &legsGrpGroup { legsGrp() };
    while (legsGrpGroup.hasNext())
    {
        legsGrpGroup.next().skip();
    }
    auto &icebergTipDelayGrpGroup { icebergTipDelayGrp() };
    while (icebergTipDelayGrpGroup.hasNext())
    {
        icebergTipDelayGrpGroup.next().skip();
    }
}

SBE_NODISCARD static SBE_CONSTEXPR bool isConstLength() SBE_NOEXCEPT
{
    return false;
}

SBE_NODISCARD static std::size_t computeLength(
    std::size_t ratePrecisionGrpLength = 0,
    std::size_t secAltIDGrpLength = 0,
    std::size_t legsGrpLength = 0,
    std::size_t icebergTipDelayGrpLength = 0)
{
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    std::size_t length = sbeBlockLength();

    length += RatePrecisionGrp::sbeHeaderSize();
    if (ratePrecisionGrpLength > 65534LL)
    {
        throw std::runtime_error("ratePrecisionGrpLength outside of allowed range [E110]");
    }
    length += ratePrecisionGrpLength *RatePrecisionGrp::sbeBlockLength();

    length += SecAltIDGrp::sbeHeaderSize();
    if (secAltIDGrpLength > 65534LL)
    {
        throw std::runtime_error("secAltIDGrpLength outside of allowed range [E110]");
    }
    length += secAltIDGrpLength *SecAltIDGrp::sbeBlockLength();

    length += LegsGrp::sbeHeaderSize();
    if (legsGrpLength > 65534LL)
    {
        throw std::runtime_error("legsGrpLength outside of allowed range [E110]");
    }
    length += legsGrpLength *LegsGrp::sbeBlockLength();

    length += IcebergTipDelayGrp::sbeHeaderSize();
    if (icebergTipDelayGrpLength > 65534LL)
    {
        throw std::runtime_error("icebergTipDelayGrpLength outside of allowed range [E110]");
    }
    length += icebergTipDelayGrpLength *IcebergTipDelayGrp::sbeBlockLength();

    return length;
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
};
}
#endif
