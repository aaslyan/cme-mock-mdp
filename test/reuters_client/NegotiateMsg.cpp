#include "NegotiateMsg.h"
#include "endian_utils.h"
#include <iostream>

void NegotiateMsg::unpackNegotiate(uint8_t* buffer)
{
    m_flowType = *((uint8_t*)&buffer[51]);

    memcpy(m_credentialsUsername, &buffer[52], 11);
    m_credentialsUsername[11] = 0;

    memcpy(m_credentialsPassword, &buffer[63], 15);
    m_credentialsPassword[15] = 0;
}

void NegotiateMsg::unpackNegotiateReject(uint8_t* buffer)
{
    m_rejectCode = *((uint8_t*)&buffer[51]);
    memset(m_reason, 0, 1024);
    m_reasonLength = ntohs(*((uint16_t*)&buffer[52]));
    memcpy(m_reason, &buffer[54], m_reasonLength);
}

void NegotiateMsg::unpack(uint8_t* buffer)
{
    unpackHeaders(buffer);
    m_msgType = *((uint8_t*)&buffer[22]);

    memcpy(m_sessionId, &buffer[23], 20);
    m_sessionId[20] = 0;

    m_timestamp = endian_utils::big_to_native<uint64_t>(*((uint64_t*)&buffer[43]));

    if (m_msgType == 'O') {
        std::cout << "Negotiation Response Msg was Decoded\n";
        unpackNegotiate(buffer);
    } else if (m_msgType == 'P') {
        std::cout << "Negotiation Reject Msg was Decoded\n";
        unpackNegotiateReject(buffer);
    } else if (m_msgType == 'N') {
        std::cout << "Negotiate Msg was Decoded\n";
        unpackNegotiate(buffer);
    }
}

void NegotiateMsg::pack()
{
    // Packing is implemented only for negotiate msg
    packHeaders();
    memset(&m_buffer[22], m_msgType, 1);
    memcpy(&m_buffer[23], m_sessionId, 20);

    uint64_t nTimestamp = endian_utils::native_to_big(m_timestamp);
    memcpy(&m_buffer[43], &nTimestamp, 8);

    memset(&m_buffer[51], m_flowType, 1);
    memcpy(&m_buffer[52], m_credentialsUsername, 11);
    memcpy(&m_buffer[63], m_credentialsPassword, 15);
}

size_t NegotiateMsg::getSize()
{
    return m_msgLength;
}

NegotiateMsg::NegotiateMsg()
{
    m_msgLength = NegotiateMsgLength; // Negotiate msg is a fixed length message
    m_rootBlockLength = 56; // Root block length is also fixed
    m_noOfGroups = 0; // 0 repeating groups
    m_noOfVarData = 0; // 0 variable length data
    m_templateId = 500;
    memset(m_sessionId, 0, 21);
}