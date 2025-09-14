#pragma once

#include "FIXMsg.h"
#include "NegotiateMsg.h"
#include <boost/endian/conversion.hpp>
#include <boost/predef/other/endian.h>
#include <chrono>

class Client {
    int m_sock = -1;
    int m_port;
    const char* m_ip;

public:
    Client(const char* ip, int port)
        : m_ip(ip)
        , m_port(port)
    {
    }
    void connectToServer();
    void sendMsg(const uint8_t* buffer, int size);
    void receiveMsg(uint8_t* buffer, int size);
    void negotiate(const char* username, const char* password);
    void establish();

    void decodeAndPrintTerminateMsg(uint8_t* buffer);
    void decodeAndPrintNegotiateResponse(uint8_t* buffer);
    void decodeAndPrintEstablishResponse(uint8_t* buffer);

    template <typename T>
    void printMsgHeader(T msg);
    ~Client();
};