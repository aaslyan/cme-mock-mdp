#include "Client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#include "NegotiateMsg.h"
#include "EstablishMsg.h"
#include "TerminateMsg.h"

void Client::connectToServer()
{
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (m_sock == -1)
    {
        std::cerr << "Unable to create socket. Exiting.\n";
        exit(-1);
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(m_port);
    inet_pton(AF_INET, m_ip, &hint.sin_addr);

    int connectRes = connect(m_sock, (sockaddr*)&hint, sizeof(hint));

    if (connectRes == -1)
    {
        std::cerr << "Unable to open a connection to remote host. Exiting.\n";
        exit(-1);
    }
}

void Client::sendMsg(const uint8_t* buffer, int size)
{
    int sendResponse = send(m_sock, buffer, size, 0);
    if (sendResponse == -1)
    {
        std::cerr << "Unable to send message. Exiting.\n";
        exit(-1);
    }
}

void Client::receiveMsg(uint8_t* buffer, int size)
{
    // memset(buffer, 0, size);
    int bytesReceived = recv(m_sock, buffer, size, 0);
    if (bytesReceived == -1)
    {
        std::cerr << "Error receiving response from the server. Exiting.\n";
        exit(-1);
    }
}

void Client::negotiate(const char* username, const char* password)
{
    NegotiateMsg msg;

    strcpy((char*)msg.m_sessionId, "qwertyui1234");
    strcpy((char*)msg.m_credentialsUsername, username);
    strcpy((char*)msg.m_credentialsPassword, password);

    msg.pack();

    sendMsg(msg.getBuffer(), msg.getSize());

    uint8_t buffer[500];
    receiveMsg(buffer, 500);

    decodeAndPrintNegotiateResponse(buffer);
}

void Client::decodeAndPrintNegotiateResponse(uint8_t* buffer)
{
    NegotiateMsg respMsg; // Succesful response is the same as the request with only the type different
    respMsg.unpack(buffer);

    if (respMsg.m_msgType == 'T')
    {
        // Response was a terminate msg
        decodeAndPrintTerminateMsg(buffer);
        std::cout << "Exiting because terminate message received\n";
        exit(-1);
    }
    else
    {
        printMsgHeader(respMsg);
        
        // Common fields to negotiation response and negotiation reject
        std::cout << "Type: " << (char)respMsg.m_msgType << '\n';
        std::cout << "Session Id: " << (const char*)respMsg.m_sessionId << '\n';
        std::cout << "Timestamp: " << respMsg.m_timestamp << '\n';

        if (respMsg.m_msgType == 'O')
        {
            std::cout << "Flow Type: " << respMsg.m_flowType << '\n';
            std::cout << "UserName: " << (char*)respMsg.m_credentialsUsername << '\n';
            std::cout << "Password: " << (char*)respMsg.m_credentialsPassword <<  '\n' << std::endl;
        }
        else
        {
            std:: cout << "Reject Code: " << respMsg.m_rejectCode << std::endl;
            std::cout << "Reason: " << respMsg.m_reason << std::endl;
        }
    }
}

template<typename T>
void Client::printMsgHeader(T msg)
{
    std::cout << "Message Length: " << msg.m_msgLength << '\n';
    std::cout << "Encoding: " << msg.m_encoding << '\n';
    std::cout << "Decryption ID: " << msg.m_decryptionId << '\n';

    std::cout << "Root Block Length: " << msg.m_rootBlockLength << '\n';
    std::cout << "Template ID: " << msg.m_templateId << '\n';
    std::cout << "Schema ID: " << msg.m_schemaId << '\n';
    std::cout << "Schema Ver: " << msg.m_schemaVer << '\n';

    std::cout << "No of Groups: " << msg.m_noOfGroups << '\n';
    std::cout << "No of Var Data: " << msg.m_noOfVarData << '\n';
}

void Client::decodeAndPrintTerminateMsg(uint8_t* buffer)
{
    TerminateMsg respMsg;
    respMsg.unpack(buffer);
    std::cout << "Terminate Message: " << '\n';
  
    printMsgHeader(respMsg);     
    std::cout << "Type: " << (char)respMsg.m_msgType << '\n';
    std::cout << "Session Id: " << (const char*)respMsg.m_sessionId << '\n';
    
    std::cout << "Termination Code: " << (char)respMsg.m_terminationCode << std::endl;
    std::cout << "Reason: " << respMsg.m_reason << std::endl;
}

void Client::decodeAndPrintEstablishResponse(uint8_t* buffer)
{
    EstablishMsg respMsg; // Succesful response is the same as the request with only the type different
    respMsg.unpack(buffer);

    if (respMsg.m_msgType == 'T')
    {
        // Response was a terminate msg
        decodeAndPrintTerminateMsg(buffer);
        std::cout << "Exiting because terminate message received\n";
        exit(-1);
    }
    else
    {
        printMsgHeader(respMsg);
        
        // Common fields to establishment ack and reject
        std::cout << "Type: " << (char)respMsg.m_msgType << '\n';
        std::cout << "Session Id: " << (const char*)respMsg.m_sessionId << '\n';
        std::cout << "Timestamp: " << respMsg.m_timestamp << '\n';

        if (respMsg.m_msgType == 'F')
        {
            std::cout << "Keep Alive Interval: " << respMsg.m_keepAliveInterval << '\n';
        }
        else
        {
            std:: cout << "Reject Code: " << respMsg.m_rejectCode << std::endl;
            std::cout << "Reason: " << respMsg.m_reason << std::endl;
        }
    }
}

void Client::establish()
{
    EstablishMsg msg;
    msg.m_keepAliveInterval = 6000;
    strcpy((char*)msg.m_sessionId, "qwertyui1234");
    msg.pack();
    sendMsg(msg.getBuffer(), msg.getSize());

    uint8_t buffer[500];
    receiveMsg(buffer, msg.m_msgLength);
    
    decodeAndPrintEstablishResponse(buffer);
}

Client::~Client()
{
    if (m_sock != -1)
        close(m_sock);
}