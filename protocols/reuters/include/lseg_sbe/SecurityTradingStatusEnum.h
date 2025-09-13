/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_SECURITYTRADINGSTATUSENUM_CXX_H_
#define _LSEG_SBE_SECURITYTRADINGSTATUSENUM_CXX_H_

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

class SecurityTradingStatusEnum
{
public:
    enum Value
    {
        Halt = static_cast<std::uint8_t>(2),
        Regular_Trading = static_cast<std::uint8_t>(17),
        Post_Close = static_cast<std::uint8_t>(26),
        Pre_Trading = static_cast<std::uint8_t>(100),
        Pause = static_cast<std::uint8_t>(111),
        No_Active_Session = static_cast<std::uint8_t>(199),
        NULL_VALUE = static_cast<std::uint8_t>(255)
    };

    static SecurityTradingStatusEnum::Value get(const std::uint8_t value)
    {
        switch (value)
        {
            case static_cast<std::uint8_t>(2): return Halt;
            case static_cast<std::uint8_t>(17): return Regular_Trading;
            case static_cast<std::uint8_t>(26): return Post_Close;
            case static_cast<std::uint8_t>(100): return Pre_Trading;
            case static_cast<std::uint8_t>(111): return Pause;
            case static_cast<std::uint8_t>(199): return No_Active_Session;
            case static_cast<std::uint8_t>(255): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum SecurityTradingStatusEnum [E103]");
    }

    static const char *c_str(const SecurityTradingStatusEnum::Value value)
    {
        switch (value)
        {
            case Halt: return "Halt";
            case Regular_Trading: return "Regular_Trading";
            case Post_Close: return "Post_Close";
            case Pre_Trading: return "Pre_Trading";
            case Pause: return "Pause";
            case No_Active_Session: return "No_Active_Session";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum SecurityTradingStatusEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, SecurityTradingStatusEnum::Value m)
    {
        return os << SecurityTradingStatusEnum::c_str(m);
    }
};

}

#endif
