/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_SECURITYREJECTREASONENUM_CXX_H_
#define _LSEG_SBE_SECURITYREJECTREASONENUM_CXX_H_

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

class SecurityRejectReasonEnum
{
public:
    enum Value
    {
        Market_Data_Subscription_Already_Requested = static_cast<char>(49),
        Security_Definition_Subscription_Already_Active = static_cast<char>(50),
        Inexistent_Security_Definition_Subscription = static_cast<char>(51),
        Incorrect_Subscription_Request_Type = static_cast<char>(52),
        Incorrect_Security_Request_Type = static_cast<char>(53),
        NULL_VALUE = static_cast<char>(0)
    };

    static SecurityRejectReasonEnum::Value get(const char value)
    {
        switch (value)
        {
            case static_cast<char>(49): return Market_Data_Subscription_Already_Requested;
            case static_cast<char>(50): return Security_Definition_Subscription_Already_Active;
            case static_cast<char>(51): return Inexistent_Security_Definition_Subscription;
            case static_cast<char>(52): return Incorrect_Subscription_Request_Type;
            case static_cast<char>(53): return Incorrect_Security_Request_Type;
            case static_cast<char>(0): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum SecurityRejectReasonEnum [E103]");
    }

    static const char *c_str(const SecurityRejectReasonEnum::Value value)
    {
        switch (value)
        {
            case Market_Data_Subscription_Already_Requested: return "Market_Data_Subscription_Already_Requested";
            case Security_Definition_Subscription_Already_Active: return "Security_Definition_Subscription_Already_Active";
            case Inexistent_Security_Definition_Subscription: return "Inexistent_Security_Definition_Subscription";
            case Incorrect_Subscription_Request_Type: return "Incorrect_Subscription_Request_Type";
            case Incorrect_Security_Request_Type: return "Incorrect_Security_Request_Type";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum SecurityRejectReasonEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, SecurityRejectReasonEnum::Value m)
    {
        return os << SecurityRejectReasonEnum::c_str(m);
    }
};

}

#endif
