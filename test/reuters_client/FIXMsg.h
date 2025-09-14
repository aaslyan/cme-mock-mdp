#pragma once
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>

constexpr size_t MAX_BUFFER_SIZE = 1024;
struct FIXMsg {

    // SOFH header fields
    uint32_t m_msgLength; // To be set by specific msg
    uint16_t m_encoding = 23520; // Big Endian
    uint32_t m_decryptionId = 0; // 0 because unencrypted

    // SBE header fields
    uint16_t m_rootBlockLength;
    uint16_t m_templateId;
    uint16_t m_schemaId = 101;
    uint16_t m_schemaVer = 22;
    uint16_t m_noOfGroups;
    uint16_t m_noOfVarData;
    uint8_t m_buffer[MAX_BUFFER_SIZE];

    void packHeaders();

    void unpackHeaders(uint8_t* buffer);
    uint8_t* getBuffer();
    virtual void pack() = 0;
    size_t getSize();
    ~FIXMsg();
};