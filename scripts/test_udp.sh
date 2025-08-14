#!/bin/bash

echo "Starting UDP test..."

# Start receiver in background
echo "Starting receiver on port 14310..."
./udp_receiver 14310 &
RECEIVER_PID=$!

# Give receiver time to start
sleep 1

# Run publisher test
echo -e "\nRunning publisher test..."
./test_udp_publisher

# Give receiver time to process final messages
sleep 1

# Stop receiver
echo -e "\nStopping receiver..."
kill $RECEIVER_PID 2>/dev/null || true

echo "Test complete!"