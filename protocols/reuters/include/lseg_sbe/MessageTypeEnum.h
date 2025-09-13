/* Generated SBE (Simple Binary Encoding) message codec */
#ifndef _LSEG_SBE_MESSAGETYPEENUM_CXX_H_
#define _LSEG_SBE_MESSAGETYPEENUM_CXX_H_

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

class MessageTypeEnum
{
public:
    enum Value
    {
        Negotiate = static_cast<char>(78),
        NegotiationResponse = static_cast<char>(79),
        NegotiationReject = static_cast<char>(80),
        Establish = static_cast<char>(69),
        EstablishmentAck = static_cast<char>(70),
        EstablishmentReject = static_cast<char>(71),
        Terminate = static_cast<char>(84),
        UnsequencedHeartbeat = static_cast<char>(72),
        Topic = static_cast<char>(65),
        Context = static_cast<char>(67),
        FinishedSending = static_cast<char>(66),
        NULL_VALUE = static_cast<char>(0)
    };

    static MessageTypeEnum::Value get(const char value)
    {
        switch (value)
        {
            case static_cast<char>(78): return Negotiate;
            case static_cast<char>(79): return NegotiationResponse;
            case static_cast<char>(80): return NegotiationReject;
            case static_cast<char>(69): return Establish;
            case static_cast<char>(70): return EstablishmentAck;
            case static_cast<char>(71): return EstablishmentReject;
            case static_cast<char>(84): return Terminate;
            case static_cast<char>(72): return UnsequencedHeartbeat;
            case static_cast<char>(65): return Topic;
            case static_cast<char>(67): return Context;
            case static_cast<char>(66): return FinishedSending;
            case static_cast<char>(0): return NULL_VALUE;
        }

        throw std::runtime_error("unknown value for enum MessageTypeEnum [E103]");
    }

    static const char *c_str(const MessageTypeEnum::Value value)
    {
        switch (value)
        {
            case Negotiate: return "Negotiate";
            case NegotiationResponse: return "NegotiationResponse";
            case NegotiationReject: return "NegotiationReject";
            case Establish: return "Establish";
            case EstablishmentAck: return "EstablishmentAck";
            case EstablishmentReject: return "EstablishmentReject";
            case Terminate: return "Terminate";
            case UnsequencedHeartbeat: return "UnsequencedHeartbeat";
            case Topic: return "Topic";
            case Context: return "Context";
            case FinishedSending: return "FinishedSending";
            case NULL_VALUE: return "NULL_VALUE";
        }

        throw std::runtime_error("unknown value for enum MessageTypeEnum [E103]:");
    }

    template<typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits> & operator << (
        std::basic_ostream<CharT, Traits> &os, MessageTypeEnum::Value m)
    {
        return os << MessageTypeEnum::c_str(m);
    }
};

}

#endif
