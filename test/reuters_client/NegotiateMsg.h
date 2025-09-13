#pragma once

#include "FIXMsg.h"
#include <chrono>
#include <boost/endian/conversion.hpp>
#include <boost/predef/other/endian.h>

constexpr size_t NegotiateMsgLength = 78;

struct NegotiateMsg : public FIXMsg
{
    NegotiateMsg();
    virtual ~NegotiateMsg(){};
    
    // Common fields to Negotiate, Negotiation Response, and Negotiation Reject
    uint8_t m_msgType = 'N';
    uint8_t m_sessionId[21]; // Session ID UUID
    uint64_t m_timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    // Negotiate and Negotiation Response Message Fields
    uint8_t m_flowType = 'U'; // Unsequenced flow type
    uint8_t m_credentialsUsername[12];
    uint8_t m_credentialsPassword[16];

    // Negotiation Reject Fields
    uint8_t m_rejectCode;
    uint16_t m_reasonLength;
    uint8_t m_reason[1024];

    size_t getSize();
    void pack();
    void unpack(uint8_t* buffer);

    void unpackNegotiate(uint8_t* buffer);
    void unpackNegotiateReject(uint8_t* buffer);

    void packNegotiate(uint8_t* buffer);
    void packNegotiateReject(uint8_t* buffer);
};
