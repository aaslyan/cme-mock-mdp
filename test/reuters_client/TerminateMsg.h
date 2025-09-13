#pragma once

#include "FIXMsg.h"
#include <chrono>
#include <boost/endian/conversion.hpp>
#include <boost/predef/other/endian.h>


struct TerminateMsg : public FIXMsg
{

    uint8_t m_msgType;
    uint8_t m_sessionId[21]; // Session ID UUID
    uint8_t m_terminationCode;
    uint16_t m_reasonLength;
    uint8_t m_reason[1024];

    void pack();
    void unpack(uint8_t* buffer);
};