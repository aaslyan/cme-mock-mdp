#!/bin/bash

echo "CME Mock Server - Market Scenario Demo"
echo "======================================"

# Function to run scenario
run_scenario() {
    local scenario=$1
    local duration=$2
    
    echo ""
    echo "Testing $scenario market scenario for $duration seconds..."
    echo "Starting server and client..."
    
    # Start server in background
    ./cme_mock_server --scenario $scenario &
    SERVER_PID=$!
    
    # Wait for server to initialize
    sleep 2
    
    # Start client in background
    ./cme_test_client &
    CLIENT_PID=$!
    
    # Run for specified duration
    sleep $duration
    
    # Stop processes
    echo "Stopping..."
    kill $SERVER_PID $CLIENT_PID 2>/dev/null
    wait $SERVER_PID $CLIENT_PID 2>/dev/null
    
    echo "Scenario $scenario completed."
    echo "----------------------------------------"
}

# Show available scenarios
echo "Available scenarios:"
./cme_mock_server --list-scenarios

# Run each scenario
echo ""
echo "Starting scenario demonstrations..."

run_scenario "normal" 5
run_scenario "fast" 5
run_scenario "thin" 5
run_scenario "recovery" 10

echo ""
echo "Demo completed!"