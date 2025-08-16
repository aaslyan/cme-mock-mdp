#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

std::atomic<bool> running(true);

void signal_handler(int sig)
{
    if (sig == SIGINT) {
        std::cout << "\nShutting down receiver..." << std::endl;
        running = false;
    }
}

int main(int argc, char* argv[])
{
    uint16_t port = 14310;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    std::cout << "UDP Receiver listening on port " << port << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;

    signal(SIGINT, signal_handler);

    // Create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Bind to port
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << port << std::endl;
        close(sock);
        return 1;
    }

    // Set receive timeout
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Receive loop
    char buffer[65536];
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    uint64_t msg_count = 0;
    uint64_t byte_count = 0;

    while (running) {
        ssize_t received = recvfrom(sock, buffer, sizeof(buffer), 0,
            (struct sockaddr*)&sender_addr, &sender_len);

        if (received > 0) {
            msg_count++;
            byte_count += received;

            // Null-terminate for display (if it's text)
            buffer[received] = '\0';

            char sender_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, INET_ADDRSTRLEN);

            std::cout << "[" << msg_count << "] From " << sender_ip
                      << ":" << ntohs(sender_addr.sin_port)
                      << " (" << received << " bytes): ";

            // Print as text if printable, otherwise as hex
            bool printable = true;
            for (ssize_t i = 0; i < received && i < 50; ++i) {
                if (buffer[i] < 32 || buffer[i] > 126) {
                    printable = false;
                    break;
                }
            }

            if (printable) {
                std::cout << buffer << std::endl;
            } else {
                std::cout << "Binary data (first 16 bytes): ";
                for (ssize_t i = 0; i < received && i < 16; ++i) {
                    printf("%02X ", (unsigned char)buffer[i]);
                }
                std::cout << "..." << std::endl;
            }
        }
    }

    close(sock);

    std::cout << "\nReceived " << msg_count << " messages, "
              << byte_count << " bytes total" << std::endl;

    return 0;
}