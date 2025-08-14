#!/bin/bash

# CME Mock Server Integration Test

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

cd "$PROJECT_ROOT"

echo "==================================="
echo "CME Mock Server Integration Test"
echo "==================================="

# Kill any existing receivers
pkill -f udp_receiver || true
sleep 0.5

# Start receivers on different ports
echo "Starting UDP receivers..."
./udp_receiver 14310 > incremental_feed.log 2>&1 &
INC_PID=$!
./udp_receiver 14320 > snapshot_feed.log 2>&1 &
SNAP_PID=$!

sleep 0.5

# Start the server
echo "Starting CME Mock Server..."
timeout 30 ./cme_mock_server config/server_config.json > server.log 2>&1 &
SERVER_PID=$!

echo "Server running for 30 seconds..."
echo "Monitoring feeds..."

# Monitor the feeds for a while
for i in {1..10}; do
    sleep 3
    echo -n "."
    
    # Check if server is still running
    if ! kill -0 $SERVER_PID 2>/dev/null; then
        echo -e "\nServer stopped unexpectedly!"
        break
    fi
done

echo -e "\n\nStopping server..."
kill $SERVER_PID 2>/dev/null || true
sleep 1

# Stop receivers
kill $INC_PID $SNAP_PID 2>/dev/null || true
wait $INC_PID $SNAP_PID 2>/dev/null || true

echo -e "\n=== Results ==="

# Analyze incremental feed
if [ -f incremental_feed.log ]; then
    INC_COUNT=$(grep -c "bytes):" incremental_feed.log || echo "0")
    echo "Incremental feed: $INC_COUNT messages received"
    echo "Sample incremental messages:"
    grep "bytes):" incremental_feed.log | head -3 || echo "No messages"
fi

# Analyze snapshot feed
if [ -f snapshot_feed.log ]; then
    SNAP_COUNT=$(grep -c "bytes):" snapshot_feed.log || echo "0")
    echo -e "\nSnapshot feed: $SNAP_COUNT messages received"
    echo "Sample snapshot messages:"
    grep "bytes):" snapshot_feed.log | head -3 || echo "No messages"
fi

# Show server statistics
echo -e "\nServer output (last 20 lines):"
tail -20 server.log || echo "No server log"

# Cleanup
rm -f incremental_feed.log snapshot_feed.log server.log

echo -e "\n==================================="
echo "Test complete!"
echo "==================================="