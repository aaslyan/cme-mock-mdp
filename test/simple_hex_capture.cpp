#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

void printHex(const std::vector<uint8_t>& data, int packet_num)
{
    std::cout << "\n=== PACKET " << packet_num << " (" << data.size() << " bytes) ===" << std::endl;

    // One-line hex for easy copying
    std::cout << "HEX: ";
    for (size_t i = 0; i < std::min(data.size(), size_t(64)); ++i) {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (unsigned)data[i] << " ";
    }
    if (data.size() > 64)
        std::cout << "...";
    std::cout << std::dec << std::endl;

    // Check for SBE headers at common offsets
    std::vector<size_t> offsets = { 0, 12, 14, 16, 20, 46 };
    for (size_t offset : offsets) {
        if (data.size() >= offset + 8) {
            uint16_t template_id = data[offset + 2] | (data[offset + 3] << 8);
            uint16_t schema_id = data[offset + 4] | (data[offset + 5] << 8);
            uint16_t version = data[offset + 6] | (data[offset + 7] << 8);

            if (template_id > 0 && template_id < 100 && schema_id == 1) {
                std::cout << "Offset " << offset << ": Template=" << template_id
                          << " Schema=" << schema_id << " Version=" << version;
                if (template_id == 46 || template_id == 52)
                    std::cout << " <-- CME!";
                std::cout << std::endl;
            }
        }
    }
}

int main()
{
    std::cout << "Simple Hex Capture Tool" << std::endl;
    std::cout << "=======================" << std::endl;

    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Set socket options for multicast
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Bind to multicast address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    addr.sin_port = htons(14310);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port 14310: " << strerror(errno) << std::endl;
        close(sock);
        return 1;
    }

    // Join multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.28.64");
    mreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        std::cerr << "Failed to join multicast group: " << strerror(errno) << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Listening on multicast 224.0.28.64:14310..." << std::endl;
    std::cout << "Waiting for packets (Ctrl+C to stop)..." << std::endl;

    int packet_count = 0;
    char buffer[2048];

    while (packet_count < 10) {
        ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes > 0) {
            packet_count++;
            std::vector<uint8_t> data(buffer, buffer + bytes);
            printHex(data, packet_count);
        } else if (bytes < 0) {
            std::cerr << "Receive error: " << strerror(errno) << std::endl;
            break;
        }
    }

    close(sock);
    std::cout << "\nCapture complete!" << std::endl;
    return 0;
}