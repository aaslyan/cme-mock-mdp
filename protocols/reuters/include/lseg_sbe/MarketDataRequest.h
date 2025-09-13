/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_MARKETDATAREQUEST_CXX_H_
#define _LSEG_SBE_MARKETDATAREQUEST_CXX_H_

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

class MarketDataRequest
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
    static constexpr std::uint16_t SBE_BLOCK_LENGTH = static_cast<std::uint16_t>(9);
    static constexpr std::uint16_t SBE_TEMPLATE_ID = static_cast<std::uint16_t>(3);
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

    MarketDataRequest() = default;

    MarketDataRequest(
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

    MarketDataRequest(char *buffer, const std::uint64_t bufferLength) :
        MarketDataRequest(buffer, 0, bufferLength, sbeBlockLength(), sbeSchemaVersion())
    {
    }

    MarketDataRequest(
        char *buffer,
        const std::uint64_t bufferLength,
        const std::uint64_t actingBlockLength,
        const std::uint64_t actingVersion) :
        MarketDataRequest(buffer, 0, bufferLength, actingBlockLength, actingVersion)
    {
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeBlockLength() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(9);
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t sbeBlockAndHeaderLength() SBE_NOEXCEPT
    {
        return messageHeader::encodedLength() + sbeBlockLength();
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t sbeTemplateId() SBE_NOEXCEPT
    {
        return static_cast<std::uint16_t>(3);
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
        return "V";
    }

    SBE_NODISCARD std::uint64_t offset() const SBE_NOEXCEPT
    {
        return m_offset;
    }

    MarketDataRequest &wrapForEncode(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
    {
        m_buffer = buffer;
        m_bufferLength = bufferLength;
        m_offset = offset;
        m_actingBlockLength = sbeBlockLength();
        m_actingVersion = sbeSchemaVersion();
        m_position = sbeCheckPosition(m_offset + m_actingBlockLength);
        return *this;
    }

    MarketDataRequest &wrapAndApplyHeader(char *buffer, const std::uint64_t offset, const std::uint64_t bufferLength)
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

    MarketDataRequest &wrapForDecode(
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

    MarketDataRequest &sbeRewind()
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
        MarketDataRequest skipper(m_buffer, m_offset, m_bufferLength, m_actingBlockLength, m_actingVersion);
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

    SBE_NODISCARD static const char *MDReqIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
    {
        switch (metaAttribute)
        {
            case MetaAttribute::SEMANTIC_TYPE: return "String";
            case MetaAttribute::PRESENCE: return "required";
            default: return "";
        }
    }

    static SBE_CONSTEXPR std::uint16_t mDReqIDId() SBE_NOEXCEPT
    {
        return 262;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDReqIDSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool mDReqIDInActingVersion() SBE_NOEXCEPT
    {
        return true;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDReqIDEncodingOffset() SBE_NOEXCEPT
    {
        return 0;
    }

    static SBE_CONSTEXPR char mDReqIDNullValue() SBE_NOEXCEPT
    {
        return static_cast<char>(0);
    }

    static SBE_CONSTEXPR char mDReqIDMinValue() SBE_NOEXCEPT
    {
        return static_cast<char>(32);
    }

    static SBE_CONSTEXPR char mDReqIDMaxValue() SBE_NOEXCEPT
    {
        return static_cast<char>(126);
    }

    static SBE_CONSTEXPR std::size_t mDReqIDEncodingLength() SBE_NOEXCEPT
    {
        return 8;
    }

    static SBE_CONSTEXPR std::uint64_t mDReqIDLength() SBE_NOEXCEPT
    {
        return 8;
    }

    SBE_NODISCARD const char *mDReqID() const SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char *mDReqID() SBE_NOEXCEPT
    {
        return m_buffer + m_offset + 0;
    }

    SBE_NODISCARD char mDReqID(const std::uint64_t index) const
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for mDReqID [E104]");
        }

        char val;
        std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
        return (val);
    }

    MarketDataRequest &mDReqID(const std::uint64_t index, const char value)
    {
        if (index >= 8)
        {
            throw std::runtime_error("index out of range for mDReqID [E105]");
        }

        char val = (value);
        std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
        return *this;
    }

    std::uint64_t getMDReqID(char *const dst, const std::uint64_t length) const
    {
        if (length > 8)
        {
            throw std::runtime_error("length too large for getMDReqID [E106]");
        }

        std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
        return length;
    }

    #ifdef SBE_USE_SPAN
    SBE_NODISCARD std::span<const char> getMDReqIDAsSpan() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        return std::span<const char>(reinterpret_cast<const char*>(buffer), 8);
    }
    #endif

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    MarketDataRequest &putMDReqID(std::span<const char, N> src) SBE_NOEXCEPT
    {
        static_assert(N <= 8, "array too large for putMDReqID");

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
    MarketDataRequest &putMDReqID(T&& src)  SBE_NOEXCEPT requires
        (std::is_pointer_v<std::remove_reference_t<T>> &&
         !std::is_array_v<std::remove_reference_t<T>>)
    #else
    MarketDataRequest &putMDReqID(const char *const src) SBE_NOEXCEPT
    #endif
    {
        std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 8);
        return *this;
    }

    #ifdef SBE_USE_SPAN
    template <std::size_t N>
    MarketDataRequest &putMDReqID(const char (&src)[N]) SBE_NOEXCEPT
    {
        return putMDReqID(std::span<const char, N>(src));
    }
    #endif

    SBE_NODISCARD std::string getMDReqIDAsString() const
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string result(buffer, length);

        return result;
    }

    std::string getMDReqIDAsJsonEscapedString()
    {
        std::ostringstream oss;
        std::string s = getMDReqIDAsString();

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
    SBE_NODISCARD std::string_view getMDReqIDAsStringView() const SBE_NOEXCEPT
    {
        const char *buffer = m_buffer + m_offset + 0;
        std::size_t length = 0;

        for (; length < 8 && *(buffer + length) != '\0'; ++length);
        std::string_view result(buffer, length);

        return result;
    }
    #endif

    #ifdef SBE_USE_STRING_VIEW
    MarketDataRequest &putMDReqID(const std::string_view str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putMDReqID [E106]");
        }

        std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
        for (std::size_t start = srcLength; start < 8; ++start)
        {
            m_buffer[m_offset + 0 + start] = 0;
        }

        return *this;
    }
    #else
    MarketDataRequest &putMDReqID(const std::string &str)
    {
        const std::size_t srcLength = str.length();
        if (srcLength > 8)
        {
            throw std::runtime_error("string too large for putMDReqID [E106]");
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

    SBE_NODISCARD SubscriptionRequestTypeEnum::Value subscriptionRequestType() const
    {
        char val;
        std::memcpy(&val, m_buffer + m_offset + 8, sizeof(char));
        return SubscriptionRequestTypeEnum::get((val));
    }

    MarketDataRequest &subscriptionRequestType(const SubscriptionRequestTypeEnum::Value value) SBE_NOEXCEPT
    {
        char val = (value);
        std::memcpy(m_buffer + m_offset + 8, &val, sizeof(char));
        return *this;
    }

    class MDEntryTypeGrp
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
        MDEntryTypeGrp() = default;

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
            dimensions.blockLength(static_cast<std::uint16_t>(1));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 1;
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
            return 1;
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

        inline MDEntryTypeGrp &next()
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


        SBE_NODISCARD static const char *MDEntryTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "char";
                case MetaAttribute::PRESENCE: return "optional";
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
            return 0;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t mDEntryTypeEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD char mDEntryTypeRaw() const SBE_NOEXCEPT
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 0, sizeof(char));
            return (val);
        }

        SBE_NODISCARD EntryTypeEnum::Value mDEntryType() const
        {
            char val;
            std::memcpy(&val, m_buffer + m_offset + 0, sizeof(char));
            return EntryTypeEnum::get((val));
        }

        MDEntryTypeGrp &mDEntryType(const EntryTypeEnum::Value value) SBE_NOEXCEPT
        {
            char val = (value);
            std::memcpy(m_buffer + m_offset + 0, &val, sizeof(char));
            return *this;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, MDEntryTypeGrp &writer)
        {
            builder << '{';
            builder << R"("MDEntryType": )";
            builder << '"' << writer.mDEntryType() << '"';

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
    MDEntryTypeGrp m_mDEntryTypeGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t MDEntryTypeGrpId() SBE_NOEXCEPT
    {
        return 35001;
    }

    SBE_NODISCARD inline MDEntryTypeGrp &mDEntryTypeGrp()
    {
        m_mDEntryTypeGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_mDEntryTypeGrp;
    }

    MDEntryTypeGrp &mDEntryTypeGrpCount(const std::uint16_t count)
    {
        m_mDEntryTypeGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_mDEntryTypeGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t mDEntryTypeGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool mDEntryTypeGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

    class RelatedSymGrp
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
        RelatedSymGrp() = default;

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
            dimensions.blockLength(static_cast<std::uint16_t>(11));
            dimensions.numInGroup(static_cast<std::uint16_t>(count));
            m_index = 0;
            m_count = count;
            m_blockLength = 11;
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
            return 11;
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

        inline RelatedSymGrp &next()
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


        SBE_NODISCARD static const char *SecurityIDMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "String";
                case MetaAttribute::PRESENCE: return "optional";
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
            return 0;
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
            return m_buffer + m_offset + 0;
        }

        SBE_NODISCARD char *securityID() SBE_NOEXCEPT
        {
            return m_buffer + m_offset + 0;
        }

        SBE_NODISCARD char securityID(const std::uint64_t index) const
        {
            if (index >= 10)
            {
                throw std::runtime_error("index out of range for securityID [E104]");
            }

            char val;
            std::memcpy(&val, m_buffer + m_offset + 0 + (index * 1), sizeof(char));
            return (val);
        }

        RelatedSymGrp &securityID(const std::uint64_t index, const char value)
        {
            if (index >= 10)
            {
                throw std::runtime_error("index out of range for securityID [E105]");
            }

            char val = (value);
            std::memcpy(m_buffer + m_offset + 0 + (index * 1), &val, sizeof(char));
            return *this;
        }

        std::uint64_t getSecurityID(char *const dst, const std::uint64_t length) const
        {
            if (length > 10)
            {
                throw std::runtime_error("length too large for getSecurityID [E106]");
            }

            std::memcpy(dst, m_buffer + m_offset + 0, sizeof(char) * static_cast<std::size_t>(length));
            return length;
        }

        #ifdef SBE_USE_SPAN
        SBE_NODISCARD std::span<const char> getSecurityIDAsSpan() const SBE_NOEXCEPT
        {
            const char *buffer = m_buffer + m_offset + 0;
            return std::span<const char>(reinterpret_cast<const char*>(buffer), 10);
        }
        #endif

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        RelatedSymGrp &putSecurityID(std::span<const char, N> src) SBE_NOEXCEPT
        {
            static_assert(N <= 10, "array too large for putSecurityID");

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
        RelatedSymGrp &putSecurityID(T&& src)  SBE_NOEXCEPT requires
            (std::is_pointer_v<std::remove_reference_t<T>> &&
             !std::is_array_v<std::remove_reference_t<T>>)
        #else
        RelatedSymGrp &putSecurityID(const char *const src) SBE_NOEXCEPT
        #endif
        {
            std::memcpy(m_buffer + m_offset + 0, src, sizeof(char) * 10);
            return *this;
        }

        #ifdef SBE_USE_SPAN
        template <std::size_t N>
        RelatedSymGrp &putSecurityID(const char (&src)[N]) SBE_NOEXCEPT
        {
            return putSecurityID(std::span<const char, N>(src));
        }
        #endif

        SBE_NODISCARD std::string getSecurityIDAsString() const
        {
            const char *buffer = m_buffer + m_offset + 0;
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
            const char *buffer = m_buffer + m_offset + 0;
            std::size_t length = 0;

            for (; length < 10 && *(buffer + length) != '\0'; ++length);
            std::string_view result(buffer, length);

            return result;
        }
        #endif

        #ifdef SBE_USE_STRING_VIEW
        RelatedSymGrp &putSecurityID(const std::string_view str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 10)
            {
                throw std::runtime_error("string too large for putSecurityID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 0, str.data(), srcLength);
            for (std::size_t start = srcLength; start < 10; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #else
        RelatedSymGrp &putSecurityID(const std::string &str)
        {
            const std::size_t srcLength = str.length();
            if (srcLength > 10)
            {
                throw std::runtime_error("string too large for putSecurityID [E106]");
            }

            std::memcpy(m_buffer + m_offset + 0, str.c_str(), srcLength);
            for (std::size_t start = srcLength; start < 10; ++start)
            {
                m_buffer[m_offset + 0 + start] = 0;
            }

            return *this;
        }
        #endif

        SBE_NODISCARD static const char *ScreenedMDRequestTypeMetaAttribute(const MetaAttribute metaAttribute) SBE_NOEXCEPT
        {
            switch (metaAttribute)
            {
                case MetaAttribute::SEMANTIC_TYPE: return "int";
                case MetaAttribute::PRESENCE: return "optional";
                default: return "";
            }
        }

        static SBE_CONSTEXPR std::uint16_t screenedMDRequestTypeId() SBE_NOEXCEPT
        {
            return 274;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t screenedMDRequestTypeSinceVersion() SBE_NOEXCEPT
        {
            return 0;
        }

        SBE_NODISCARD bool screenedMDRequestTypeInActingVersion() SBE_NOEXCEPT
        {
            return true;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t screenedMDRequestTypeEncodingOffset() SBE_NOEXCEPT
        {
            return 10;
        }

        SBE_NODISCARD static SBE_CONSTEXPR std::size_t screenedMDRequestTypeEncodingLength() SBE_NOEXCEPT
        {
            return 1;
        }

        SBE_NODISCARD std::uint8_t screenedMDRequestTypeRaw() const SBE_NOEXCEPT
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 10, sizeof(std::uint8_t));
            return (val);
        }

        SBE_NODISCARD ScreenedMDRequestTypeEnum::Value screenedMDRequestType() const
        {
            std::uint8_t val;
            std::memcpy(&val, m_buffer + m_offset + 10, sizeof(std::uint8_t));
            return ScreenedMDRequestTypeEnum::get((val));
        }

        RelatedSymGrp &screenedMDRequestType(const ScreenedMDRequestTypeEnum::Value value) SBE_NOEXCEPT
        {
            std::uint8_t val = (value);
            std::memcpy(m_buffer + m_offset + 10, &val, sizeof(std::uint8_t));
            return *this;
        }

        template<typename CharT, typename Traits>
        friend std::basic_ostream<CharT, Traits> & operator << (
            std::basic_ostream<CharT, Traits> &builder, RelatedSymGrp &writer)
        {
            builder << '{';
            builder << R"("SecurityID": )";
            builder << '"' <<
                writer.getSecurityIDAsJsonEscapedString().c_str() << '"';

            builder << ", ";
            builder << R"("ScreenedMDRequestType": )";
            builder << '"' << writer.screenedMDRequestType() << '"';

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
    RelatedSymGrp m_relatedSymGrp;

public:
    SBE_NODISCARD static SBE_CONSTEXPR std::uint16_t RelatedSymGrpId() SBE_NOEXCEPT
    {
        return 35002;
    }

    SBE_NODISCARD inline RelatedSymGrp &relatedSymGrp()
    {
        m_relatedSymGrp.wrapForDecode(m_buffer, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_relatedSymGrp;
    }

    RelatedSymGrp &relatedSymGrpCount(const std::uint16_t count)
    {
        m_relatedSymGrp.wrapForEncode(m_buffer, count, sbePositionPtr(), m_actingVersion, m_bufferLength);
        return m_relatedSymGrp;
    }

    SBE_NODISCARD static SBE_CONSTEXPR std::uint64_t relatedSymGrpSinceVersion() SBE_NOEXCEPT
    {
        return 0;
    }

    SBE_NODISCARD bool relatedSymGrpInActingVersion() const SBE_NOEXCEPT
    {
        return true;
    }

template<typename CharT, typename Traits>
friend std::basic_ostream<CharT, Traits> & operator << (
    std::basic_ostream<CharT, Traits> &builder, const MarketDataRequest &_writer)
{
    MarketDataRequest writer(
        _writer.m_buffer,
        _writer.m_offset,
        _writer.m_bufferLength,
        _writer.m_actingBlockLength,
        _writer.m_actingVersion);

    builder << '{';
    builder << R"("Name": "MarketDataRequest", )";
    builder << R"("sbeTemplateId": )";
    builder << writer.sbeTemplateId();
    builder << ", ";

    builder << R"("MDReqID": )";
    builder << '"' <<
        writer.getMDReqIDAsJsonEscapedString().c_str() << '"';

    builder << ", ";
    builder << R"("SubscriptionRequestType": )";
    builder << '"' << writer.subscriptionRequestType() << '"';

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("MDEntryTypeGrp": [)";
        writer.mDEntryTypeGrp().forEach(
            [&](MDEntryTypeGrp &mDEntryTypeGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << mDEntryTypeGrp;
            });
        builder << ']';
    }

    builder << ", ";
    {
        bool atLeastOne = false;
        builder << R"("RelatedSymGrp": [)";
        writer.relatedSymGrp().forEach(
            [&](RelatedSymGrp &relatedSymGrp)
            {
                if (atLeastOne)
                {
                    builder << ", ";
                }
                atLeastOne = true;
                builder << relatedSymGrp;
            });
        builder << ']';
    }

    builder << '}';

    return builder;
}

void skip()
{
    auto &mDEntryTypeGrpGroup { mDEntryTypeGrp() };
    while (mDEntryTypeGrpGroup.hasNext())
    {
        mDEntryTypeGrpGroup.next().skip();
    }
    auto &relatedSymGrpGroup { relatedSymGrp() };
    while (relatedSymGrpGroup.hasNext())
    {
        relatedSymGrpGroup.next().skip();
    }
}

SBE_NODISCARD static SBE_CONSTEXPR bool isConstLength() SBE_NOEXCEPT
{
    return false;
}

SBE_NODISCARD static std::size_t computeLength(
    std::size_t mDEntryTypeGrpLength = 0,
    std::size_t relatedSymGrpLength = 0)
{
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
    std::size_t length = sbeBlockLength();

    length += MDEntryTypeGrp::sbeHeaderSize();
    if (mDEntryTypeGrpLength > 65534LL)
    {
        throw std::runtime_error("mDEntryTypeGrpLength outside of allowed range [E110]");
    }
    length += mDEntryTypeGrpLength *MDEntryTypeGrp::sbeBlockLength();

    length += RelatedSymGrp::sbeHeaderSize();
    if (relatedSymGrpLength > 65534LL)
    {
        throw std::runtime_error("relatedSymGrpLength outside of allowed range [E110]");
    }
    length += relatedSymGrpLength *RelatedSymGrp::sbeBlockLength();

    return length;
#if defined(__GNUG__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
};
}
#endif
