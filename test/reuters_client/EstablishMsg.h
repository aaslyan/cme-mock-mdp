#pragma once

#include "FIXMsg.h"
#include <boost/endian/conversion.hpp>
#include <boost/predef/other/endian.h>
#include <chrono>

struct EstablishMsg : public FIXMsg {

    EstablishMsg();
    virtual ~EstablishMsg() { }

    // Common fields
    uint8_t m_msgType = 'E';
    uint8_t m_sessionId[21]; // Session ID UUID
    uint64_t m_timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // Fields common to Establish and Establishment Ack Messages
    uint32_t m_keepAliveInterval;

    // Fields of Establishment Reject Message
    uint8_t m_rejectCode;
    uint8_t m_reason[1024];
    uint16_t m_reasonLength;

    void pack();
    void unpack(uint8_t* buffer);

    void unpackEstablish(uint8_t* buffer);
    void unpackEstablishReject(uint8_t* buffer);

    void packEstablish(uint8_t* buffer);
    void packEstablishReject(uint8_t* buffer);
};