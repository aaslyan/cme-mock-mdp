/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_FLOWTYPEENUM_CXX_H_
#define _LSEG_SBE_FLOWTYPEENUM_CXX_H_

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

class FlowTypeEnum
{
public:
    enum Value
    {
        Recoverable = static_cast<char>(82),
        Unsequenced = static_cast<char>(85),
        Idempotent = static_cast<char>(73),
        None = static_cast<char>(78),
        NULL_VALUE = static_cast<char>(0)
    };

    static FlowTypeEnum::Value get(const char value)
    {
        switch (value)
        {
            case static_cast<char>(82): return Recoverable;
            case static_cast<char>(85): return Unsequenced;
            case static_cast<char>(73): return Idempotent;
            case static_cast<char>(78): return None;
            case static_cast<char>(0): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum FlowTypeEnum [E103]");
    }

    static const char *c_str(const FlowTypeEnum::Value value)
    {
        switch (value)
        {
            case Recoverable: return "Recoverable";
            case Unsequenced: return "Unsequenced";
            case Idempotent: return "Idempotent";
            case None: return "None";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum FlowTypeEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, FlowTypeEnum::Value m)
    {
        return os << FlowTypeEnum::c_str(m);
    }
};

}

#endif
