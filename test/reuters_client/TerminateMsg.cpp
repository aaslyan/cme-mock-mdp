#include "TerminateMsg.h"
#include <iostream>
void TerminateMsg::unpack(uint8_t* buffer)
{
    unpackHeaders(buffer);
    m_msgType = *((uint8_t *)&buffer[22]);
    
    memcpy(m_sessionId, &buffer[23], 20);
    m_sessionId[20] = 0;

    m_terminationCode = *((uint8_t*)&buffer[43]);
    
    m_reasonLength = ntohs(*((uint16_t*)&buffer[44]));

    memset(m_reason, 0, 1024);
    memcpy(m_reason, &buffer[46], m_reasonLength);
}

void TerminateMsg::pack()
{
    packHeaders();
    memset(&m_buffer[22], htons(m_msgType), 1);
    memcpy(&m_buffer[23], m_sessionId, 20);
    memcpy(&m_buffer[43], &m_terminationCode, 1);
    memcpy(&m_buffer[44], &m_reason, 1024);
}
