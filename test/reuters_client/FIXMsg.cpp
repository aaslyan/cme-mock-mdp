#include "FIXMsg.h"

void FIXMsg::packHeaders()
{   
    memset(m_buffer, 0, 1024);
    uint32_t nMsgLength = htonl(m_msgLength);
    memcpy(m_buffer, &nMsgLength, sizeof(nMsgLength));

    uint16_t nEncoding = htons(m_encoding);
    memcpy(&m_buffer[4], &nEncoding, 2);
    
    uint32_t nDecryptionID = htons(m_decryptionId);
    memcpy(&m_buffer[6], &nDecryptionID, 4);

    uint16_t nRootBlockLength = htons(m_rootBlockLength);
    memcpy(&m_buffer[10], &nRootBlockLength, 2);

    uint16_t nTemplateID = htons(m_templateId);
    memcpy(&m_buffer[12], &nTemplateID, 2);

    uint16_t nSchemaID = htons(m_schemaId);
    memcpy(&m_buffer[14], &nSchemaID, 2);

    uint16_t nSchemaVer = htons(m_schemaVer);
    memcpy(&m_buffer[16],  &nSchemaVer , 2);

    uint16_t nNoOfGroups = htons(m_noOfGroups);
    memcpy(&m_buffer[18], &nNoOfGroups, 2);

    uint16_t nNoOfVarData = htons(m_noOfVarData);
    memcpy(&m_buffer[20], &nNoOfVarData, 2);
}

void FIXMsg::unpackHeaders(uint8_t* buffer)
{
    m_msgLength = ntohl(*((uint32_t*)buffer));
    m_encoding = ntohs(*((u_int16_t*)&buffer[4]));
    m_decryptionId = ntohl(*((u_int32_t*)&buffer[6]));
    
    m_rootBlockLength = ntohs(*((uint16_t*)&buffer[10]));
    m_templateId = ntohs(*((u_int16_t*)&buffer[12]));
    m_schemaId = ntohs(*((u_int16_t*)&buffer[14]));
    m_schemaVer = ntohs(*((u_int16_t*)&buffer[16]));
    m_noOfGroups = ntohs(*((u_int16_t*)&buffer[18]));
    m_noOfVarData = ntohs(*((u_int16_t*)&buffer[20]));
}

FIXMsg::~FIXMsg()
{

}

uint8_t* FIXMsg::getBuffer()
{
    return m_buffer;
}

size_t FIXMsg::getSize()
{
    return m_msgLength;
}