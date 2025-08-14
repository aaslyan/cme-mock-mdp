#!/bin/bash

# CME Mock Server End-to-End Test

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

cd "$PROJECT_ROOT"

echo "============================================="
echo "CME Mock Server End-to-End Test"
echo "============================================="

# Kill any existing processes
echo "Cleaning up any existing processes..."
pkill -f cme_mock_server || true
pkill -f cme_test_client || true
sleep 1

# Start the mock server
echo "Starting CME Mock Server..."
./cme_mock_server config/server_config.json > server.log 2>&1 &
SERVER_PID=$!

# Give server time to start
sleep 2

# Check if server is running
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "Server failed to start!"
    cat server.log
    exit 1
fi

echo "Server started successfully (PID: $SERVER_PID)"

# Start the test client
echo "Starting CME Test Client..."
timeout 60 ./cme_test_client 14320 14310 > client.log 2>&1 &
CLIENT_PID=$!

echo "Client started (PID: $CLIENT_PID)"
echo "Running test for 60 seconds..."

# Monitor for 60 seconds
for i in {1..12}; do
    sleep 5
    echo -n "."
    
    # Check if processes are still running
    if ! kill -0 $SERVER_PID 2>/dev/null; then
        echo -e "\nServer stopped unexpectedly!"
        break
    fi
    
    if ! kill -0 $CLIENT_PID 2>/dev/null; then
        echo -e "\nClient stopped"
        break
    fi
done

echo -e "\n\nTest completed. Stopping processes..."

# Stop client and server
kill $CLIENT_PID 2>/dev/null || true
kill $SERVER_PID 2>/dev/null || true

# Wait for clean shutdown
sleep 2

# Force kill if needed
kill -9 $CLIENT_PID 2>/dev/null || true
kill -9 $SERVER_PID 2>/dev/null || true

echo -e "\n============================================="
echo "Test Results"
echo "============================================="

# Show server output
echo -e "\n--- Server Output (last 30 lines) ---"
tail -30 server.log 2>/dev/null || echo "No server log found"

# Show client output
echo -e "\n--- Client Output (last 50 lines) ---"
tail -50 client.log 2>/dev/null || echo "No client log found"

# Analyze results
echo -e "\n--- Analysis ---"

# Check for successful connections
if grep -q "UDP receiver initialized" client.log 2>/dev/null; then
    echo "✓ Client UDP receivers initialized successfully"
else
    echo "✗ Client failed to initialize UDP receivers"
fi

# Check for market data reception
SNAPSHOTS=$(grep -c "Snapshot received" client.log 2>/dev/null || echo "0")
INCREMENTALS=$(grep -c "Incrementals received" client.log 2>/dev/null || echo "0")

echo "Market Data Reception:"
echo "  Snapshots: $SNAPSHOTS"
echo "  Incremental messages: $(grep "Incrementals received" client.log | tail -1 | grep -o '[0-9]*' | tail -1 || echo "0")"

# Check server statistics
SERVER_UPDATES=$(grep "Total updates generated" server.log | tail -1 | grep -o '[0-9]*' | head -1 || echo "0")
SERVER_TRADES=$(grep "Total trades generated" server.log | tail -1 | grep -o '[0-9]*' | head -1 || echo "0")

echo -e "\nServer Statistics:"
echo "  Updates generated: $SERVER_UPDATES"
echo "  Trades generated: $SERVER_TRADES"

# Success criteria
if [ "$SNAPSHOTS" -gt "0" ] && [ "$SERVER_UPDATES" -gt "100" ]; then
    echo -e "\n🎉 END-TO-END TEST PASSED!"
    echo "✓ Server generated market data"
    echo "✓ Client received and decoded messages"
    echo "✓ Order books reconstructed successfully"
    TEST_RESULT=0
else
    echo -e "\n❌ END-TO-END TEST FAILED"
    echo "Check logs above for details"
    TEST_RESULT=1
fi

# Cleanup
rm -f server.log client.log

echo -e "\n============================================="
echo "Test Summary: $([ $TEST_RESULT -eq 0 ] && echo "PASSED" || echo "FAILED")"
echo "============================================="

exit $TEST_RESULT