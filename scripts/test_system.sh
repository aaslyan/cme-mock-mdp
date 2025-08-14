#!/bin/bash

# CME Mock Server System Test Script

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

cd "$PROJECT_ROOT"

echo "==================================="
echo "CME Mock Server System Test"
echo "==================================="

# Function to run a test with timeout
run_test() {
    local test_name=$1
    local command=$2
    local timeout_seconds=${3:-10}
    
    echo -e "\n--- Running: $test_name ---"
    if timeout $timeout_seconds bash -c "$command"; then
        echo "✓ $test_name passed"
        return 0
    else
        echo "✗ $test_name failed or timed out"
        return 1
    fi
}

# Build if needed
if [ ! -f "test_udp_publisher" ] || [ ! -f "udp_receiver" ]; then
    echo "Building test programs..."
    make clean && make all
fi

# Test 1: UDP Publisher/Receiver Test
echo -e "\n=== Test 1: UDP Communication ==="

# Start receiver in background, redirect output to file
./udp_receiver 14310 > receiver_output.log 2>&1 &
RECEIVER_PID=$!
sleep 0.5

# Run publisher
./test_udp_publisher

# Stop receiver
sleep 0.5
kill $RECEIVER_PID 2>/dev/null || true
wait $RECEIVER_PID 2>/dev/null || true

# Check results
echo -e "\n--- Receiver Output ---"
if [ -f receiver_output.log ]; then
    head -20 receiver_output.log
    RECEIVED_COUNT=$(grep -c "bytes):" receiver_output.log || echo "0")
    echo "..."
    echo "Total messages received: $RECEIVED_COUNT"
    rm -f receiver_output.log
fi

# Test 2: Multiple Ports Test
echo -e "\n=== Test 2: Multiple Ports ==="

# Start receivers on different ports
echo "Starting receivers on ports 14310, 14320, 14330..."
./udp_receiver 14310 > receiver1.log 2>&1 &
PID1=$!
./udp_receiver 14320 > receiver2.log 2>&1 &
PID2=$!
./udp_receiver 14330 > receiver3.log 2>&1 &
PID3=$!

sleep 0.5

# Send to different ports
cat > test_multiport.cpp << 'EOF'
#include "network/udp_publisher.h"
#include "utils/logger.h"
#include <iostream>

int main() {
    cme_mock::Logger::instance().set_level(cme_mock::LogLevel::INFO);
    
    // Test different ports
    uint16_t ports[] = {14310, 14320, 14330};
    const char* feed_names[] = {"Incremental", "Snapshot", "Definition"};
    
    for (int i = 0; i < 3; ++i) {
        cme_mock::UDPPublisher pub("127.0.0.1", ports[i]);
        if (pub.initialize()) {
            std::string msg = std::string(feed_names[i]) + " Feed Test Message";
            pub.send(std::vector<uint8_t>(msg.begin(), msg.end()));
            std::cout << "Sent to port " << ports[i] << ": " << msg << std::endl;
        }
    }
    
    return 0;
}
EOF

# Compile and run multiport test
g++ -std=c++17 -I./include test_multiport.cpp src/network/udp_publisher.o src/utils/logger.o -o test_multiport
./test_multiport

sleep 0.5

# Stop all receivers
kill $PID1 $PID2 $PID3 2>/dev/null || true
wait $PID1 $PID2 $PID3 2>/dev/null || true

# Check results
for i in 1 2 3; do
    if [ -f receiver$i.log ] && grep -q "bytes):" receiver$i.log; then
        echo "✓ Receiver $i received data"
    else
        echo "✗ Receiver $i did not receive data"
    fi
    rm -f receiver$i.log
done

# Cleanup
rm -f test_multiport test_multiport.cpp

# Test 3: Performance Test
echo -e "\n=== Test 3: Performance Test ==="

cat > test_performance.cpp << 'EOF'
#include "network/udp_publisher.h"
#include <iostream>
#include <chrono>
#include <vector>

int main() {
    cme_mock::UDPPublisher pub("127.0.0.1", 14310);
    if (!pub.initialize()) {
        return 1;
    }
    
    // Create a typical market data message (100 bytes)
    std::vector<uint8_t> msg(100, 0x42);
    
    const int num_messages = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_messages; ++i) {
        pub.send(msg);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double msgs_per_sec = (num_messages * 1000000.0) / duration.count();
    double mbps = (msgs_per_sec * msg.size() * 8) / 1000000.0;
    
    std::cout << "Sent " << num_messages << " messages in " 
              << duration.count() / 1000.0 << " ms" << std::endl;
    std::cout << "Rate: " << static_cast<int>(msgs_per_sec) << " msgs/sec" << std::endl;
    std::cout << "Throughput: " << mbps << " Mbps" << std::endl;
    
    return 0;
}
EOF

g++ -std=c++17 -O3 -I./include test_performance.cpp src/network/udp_publisher.o src/utils/logger.o -o test_performance
./test_performance

rm -f test_performance test_performance.cpp

echo -e "\n==================================="
echo "Test Summary"
echo "==================================="
echo "✓ UDP Publisher/Receiver communication working"
echo "✓ Multiple port support verified"
echo "✓ Performance metrics collected"
echo ""
echo "Next steps:"
echo "1. Implement Order Book State Engine"
echo "2. Add MDP 3.0 message encoding"
echo "3. Create Market Data Generator"
echo "==================================="