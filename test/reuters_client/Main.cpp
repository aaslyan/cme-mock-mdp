#include "Client.h"

int main()
{
    // Set the market data gateway ip port and login credentials.
    const char* ip = "127.0.0.1";
    int port = 11501;
    const char* username = "CISA_MD_01";
    const char* password = "mit123";

    // Create the client object and connect to the gateway.
    Client client(ip, port);
    client.connectToServer();
    client.negotiate(username, password); // Send Negotiate msg.
    client.establish(); // Send Establish msg.
}
