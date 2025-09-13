/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_SESSIONREJECTCODEENUM_CXX_H_
#define _LSEG_SBE_SESSIONREJECTCODEENUM_CXX_H_

#if !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS 1
#endif

#include <cstdint>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <string>

#define SBE_NULLVALUE_INT8 (std::numeric_limits<std::int8_t>::min)()
#define SBE_NULLVALUE_INT16 (std::numeric_limits<std::int16_t>::min)()
#define SBE_NULLVALUE_INT32 (std::numeric_limits<std::int32_t>::min)()
#define SBE_NULLVALUE_INT64 (std::numeric_limits<std::int64_t>::min)()
#define SBE_NULLVALUE_UINT8 (std::numeric_limits<std::uint8_t>::max)()
#define SBE_NULLVALUE_UINT16 (std::numeric_limits<std::uint16_t>::max)()
#define SBE_NULLVALUE_UINT32 (std::numeric_limits<std::uint32_t>::max)()
#define SBE_NULLVALUE_UINT64 (std::numeric_limits<std::uint64_t>::max)()

namespace lseg_sbe {

class SessionRejectCodeEnum
{
public:
    enum Value
    {
        SessionBlocked = static_cast<char>(66),
        Credentials = static_cast<char>(67),
        DuplicateId = static_cast<char>(68),
        AlreadyEstablished = static_cast<char>(69),
        FlowTypeNotSupported = static_cast<char>(70),
        KeepaliveInterval = static_cast<char>(75),
        Unnegotiated = static_cast<char>(78),
        Unspecified = static_cast<char>(85),
        NULL_VALUE = static_cast<char>(0)
    };

    static SessionRejectCodeEnum::Value get(const char value)
    {
        switch (value)
        {
            case static_cast<char>(66): return SessionBlocked;
            case static_cast<char>(67): return Credentials;
            case static_cast<char>(68): return DuplicateId;
            case static_cast<char>(69): return AlreadyEstablished;
            case static_cast<char>(70): return FlowTypeNotSupported;
            case static_cast<char>(75): return KeepaliveInterval;
            case static_cast<char>(78): return Unnegotiated;
            case static_cast<char>(85): return Unspecified;
            case static_cast<char>(0): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum SessionRejectCodeEnum [E103]");
    }

    static const char *c_str(const SessionRejectCodeEnum::Value value)
    {
        switch (value)
        {
            case SessionBlocked: return "SessionBlocked";
            case Credentials: return "Credentials";
            case DuplicateId: return "DuplicateId";
            case AlreadyEstablished: return "AlreadyEstablished";
            case FlowTypeNotSupported: return "FlowTypeNotSupported";
            case KeepaliveInterval: return "KeepaliveInterval";
            case Unnegotiated: return "Unnegotiated";
            case Unspecified: return "Unspecified";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum SessionRejectCodeEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, SessionRejectCodeEnum::Value m)
    {
        return os << SessionRejectCodeEnum::c_str(m);
    }
};

}

#endif
