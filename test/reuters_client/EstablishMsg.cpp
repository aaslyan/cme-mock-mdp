#include "EstablishMsg.h"
#include <iostream>
void EstablishMsg::unpack(uint8_t* buffer)
{
    unpackHeaders(buffer);
    m_msgType = *((uint8_t *)&buffer[22]);
    memcpy(m_sessionId, &buffer[23], 20);
    m_sessionId[20] = 0;
    m_timestamp = boost::endian::big_to_native<uint64_t>(*((uint64_t*)&buffer[43]));
    
    if (m_msgType == 'F')
    {
        std::cout << "Establishment Ack Msg was Decoded\n";
        unpackEstablish(buffer);
    }
    else if (m_msgType == 'G')
    {
        unpackEstablishReject(buffer);
        std::cout << "Establishment Reject Msg was Decoded\n";
    }
    else if (m_msgType == 'E')
    {
        std::cout << "Establish Msg was Decoded\n";
        unpackEstablish(buffer);
    }
}

void EstablishMsg::unpackEstablish(uint8_t* buffer)
{
    m_keepAliveInterval = ntohl(*((uint32_t*)&buffer[51]));
}

void EstablishMsg::unpackEstablishReject(uint8_t* buffer)
{
    m_rejectCode = *((uint8_t*)&buffer[51]);
    m_reasonLength = ntohs(*((uint16_t*)&buffer[52]));
    memset(m_reason, 0, 1024);
    memcpy(m_reason, &buffer[54], m_reasonLength);
}

void EstablishMsg::pack()
{
    // Packing is implemented only for establish msg
    packHeaders();
    memset(&m_buffer[22], m_msgType, 1);
    memcpy(&m_buffer[23], m_sessionId, 20);

    uint64_t nTimestamp = boost::endian::native_to_big(m_timestamp);
    memcpy(&m_buffer[43], &nTimestamp, 8);
    
    uint32_t nKeepAliveInterval = htonl(m_keepAliveInterval);
    memcpy(&m_buffer[51], &nKeepAliveInterval, 4);
}


EstablishMsg::EstablishMsg()
{
    m_msgLength = 55; // Establish msg is a fixed length message
    m_rootBlockLength = 33; // Root block length is also fixed
    m_templateId = 501;
    m_noOfGroups = 0; // No repeating groups
    m_noOfVarData = 0; // No variable length fields
    memset(m_sessionId, 0, 21);
}