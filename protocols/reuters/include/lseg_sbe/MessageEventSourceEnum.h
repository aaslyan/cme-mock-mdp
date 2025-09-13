/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_MESSAGEEVENTSOURCEENUM_CXX_H_
#define _LSEG_SBE_MESSAGEEVENTSOURCEENUM_CXX_H_

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

class MessageEventSourceEnum
{
public:
    enum Value
    {
        Normal = static_cast<std::uint8_t>(0),
        MDReplenishment = static_cast<std::uint8_t>(1),
        BufferedConnection = static_cast<std::uint8_t>(2),
        PeriodicMDUpdate = static_cast<std::uint8_t>(3),
        NULL_VALUE = static_cast<std::uint8_t>(255)
    };

    static MessageEventSourceEnum::Value get(const std::uint8_t value)
    {
        switch (value)
        {
            case static_cast<std::uint8_t>(0): return Normal;
            case static_cast<std::uint8_t>(1): return MDReplenishment;
            case static_cast<std::uint8_t>(2): return BufferedConnection;
            case static_cast<std::uint8_t>(3): return PeriodicMDUpdate;
            case static_cast<std::uint8_t>(255): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum MessageEventSourceEnum [E103]");
    }

    static const char *c_str(const MessageEventSourceEnum::Value value)
    {
        switch (value)
        {
            case Normal: return "Normal";
            case MDReplenishment: return "MDReplenishment";
            case BufferedConnection: return "BufferedConnection";
            case PeriodicMDUpdate: return "PeriodicMDUpdate";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum MessageEventSourceEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, MessageEventSourceEnum::Value m)
    {
        return os << MessageEventSourceEnum::c_str(m);
    }
};

}

#endif
