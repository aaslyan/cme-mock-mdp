/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_HALTREASONENUM_CXX_H_
#define _LSEG_SBE_HALTREASONENUM_CXX_H_

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

class HaltReasonEnum
{
public:
    enum Value
    {
        News_Pending = static_cast<std::uint8_t>(4),
        Other = static_cast<std::uint8_t>(100),
        Circuit_Breaker = static_cast<std::uint8_t>(101),
        Fast_Market = static_cast<std::uint8_t>(102),
        System_Problem = static_cast<std::uint8_t>(103),
        Instrument_Status_Change = static_cast<std::uint8_t>(104),
        System_Failure = static_cast<std::uint8_t>(105),
        Invalid_Instrument_Setup = static_cast<std::uint8_t>(106),
        Clearing_Halt = static_cast<std::uint8_t>(107),
        NULL_VALUE = static_cast<std::uint8_t>(255)
    };

    static HaltReasonEnum::Value get(const std::uint8_t value)
    {
        switch (value)
        {
            case static_cast<std::uint8_t>(4): return News_Pending;
            case static_cast<std::uint8_t>(100): return Other;
            case static_cast<std::uint8_t>(101): return Circuit_Breaker;
            case static_cast<std::uint8_t>(102): return Fast_Market;
            case static_cast<std::uint8_t>(103): return System_Problem;
            case static_cast<std::uint8_t>(104): return Instrument_Status_Change;
            case static_cast<std::uint8_t>(105): return System_Failure;
            case static_cast<std::uint8_t>(106): return Invalid_Instrument_Setup;
            case static_cast<std::uint8_t>(107): return Clearing_Halt;
            case static_cast<std::uint8_t>(255): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum HaltReasonEnum [E103]");
    }

    static const char *c_str(const HaltReasonEnum::Value value)
    {
        switch (value)
        {
            case News_Pending: return "News_Pending";
            case Other: return "Other";
            case Circuit_Breaker: return "Circuit_Breaker";
            case Fast_Market: return "Fast_Market";
            case System_Problem: return "System_Problem";
            case Instrument_Status_Change: return "Instrument_Status_Change";
            case System_Failure: return "System_Failure";
            case Invalid_Instrument_Setup: return "Invalid_Instrument_Setup";
            case Clearing_Halt: return "Clearing_Halt";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum HaltReasonEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, HaltReasonEnum::Value m)
    {
        return os << HaltReasonEnum::c_str(m);
    }
};

}

#endif
