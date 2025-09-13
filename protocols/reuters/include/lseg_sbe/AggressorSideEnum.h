/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_AGGRESSORSIDEENUM_CXX_H_
#define _LSEG_SBE_AGGRESSORSIDEENUM_CXX_H_

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

class AggressorSideEnum
{
public:
    enum Value
    {
        Buyer = static_cast<char>(49),
        Seller = static_cast<char>(50),
        NULL_VALUE = static_cast<char>(0)
    };

    static AggressorSideEnum::Value get(const char value)
    {
        switch (value)
        {
            case static_cast<char>(49): return Buyer;
            case static_cast<char>(50): return Seller;
            case static_cast<char>(0): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum AggressorSideEnum [E103]");
    }

    static const char *c_str(const AggressorSideEnum::Value value)
    {
        switch (value)
        {
            case Buyer: return "Buyer";
            case Seller: return "Seller";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum AggressorSideEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, AggressorSideEnum::Value m)
    {
        return os << AggressorSideEnum::c_str(m);
    }
};

}

#endif
