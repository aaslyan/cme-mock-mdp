#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void testUDPConnection() {
    std::cout << "Network Connectivity Test" << std::endl;
    std::cout << "========================" << std::endl;
    
    // Test if we can create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "ERROR: Cannot create UDP socket" << std::endl;
        return;
    }
    std::cout << "✓ UDP socket created successfully" << std::endl;
    
    // Test binding to port 14310
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(14310);
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "ERROR: Cannot bind to 127.0.0.1:14310 - " << strerror(errno) << std::endl;
        std::cerr << "This usually means:" << std::endl;
        std::cerr << "  1. Port is already in use (mock server running?)" << std::endl;
        std::cerr << "  2. Permission denied" << std::endl;
        std::cerr << "  3. Network interface issue" << std::endl;
    } else {
        std::cout << "✓ Successfully bound to 127.0.0.1:14310" << std::endl;
    }
    
    close(sock);
    
    // Test multicast addresses
    std::cout << "\nTesting multicast addresses from config:" << std::endl;
    std::cout << "Incremental feed: 224.0.28.64:14310" << std::endl;
    std::cout << "Snapshot feed: 224.0.28.69:14320" << std::endl;
    
    std::cout << "\nRecommendations:" << std::endl;
    std::cout << "1. Make sure mock server is running" << std::endl;
    std::cout << "2. Check if server is sending to multicast or unicast" << std::endl;
    std::cout << "3. Try listening on multicast address instead" << std::endl;
}

int main() {
    testUDPConnection();
    return 0;
}