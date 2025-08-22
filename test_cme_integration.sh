#!/bin/bash

# CME Mock Server Integration Test Script
# Tests the mock server and test client end-to-end compatibility

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
TEST_IP="127.0.0.1"
TEST_PORT="20001"
SERVER_PID=""
CLIENT_PID=""
TEST_DURATION=10  # seconds
CLEANUP_DONE=false

# Cleanup function
cleanup() {
    if [ "$CLEANUP_DONE" = true ]; then
        return
    fi
    CLEANUP_DONE=true
    
    echo -e "\n${YELLOW}Cleaning up processes...${NC}"
    
    if [ ! -z "$CLIENT_PID" ] && kill -0 "$CLIENT_PID" 2>/dev/null; then
        echo "Stopping test client (PID: $CLIENT_PID)"
        kill -TERM "$CLIENT_PID" 2>/dev/null || true
        wait "$CLIENT_PID" 2>/dev/null || true
    fi
    
    if [ ! -z "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
        echo "Stopping mock server (PID: $SERVER_PID)"
        kill -TERM "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
    fi
    
    # Clean up any remaining processes
    pkill -f "cme_mock_server" 2>/dev/null || true
    pkill -f "test_client" 2>/dev/null || true
    
    echo -e "${GREEN}Cleanup completed${NC}"
}

# Set up signal handlers
trap cleanup EXIT INT TERM

print_header() {
    echo -e "${BLUE}================================================${NC}"
    echo -e "${BLUE}CME Mock Server Integration Test${NC}"
    echo -e "${BLUE}================================================${NC}"
    echo -e "Test IP: ${TEST_IP}"
    echo -e "Test Port: ${TEST_PORT}"
    echo -e "Test Duration: ${TEST_DURATION} seconds"
    echo ""
}

check_prerequisites() {
    echo -e "${YELLOW}Checking prerequisites...${NC}"
    
    # Check if binaries exist
    if [ ! -f "./cme_mock_server" ]; then
        echo -e "${RED}Error: cme_mock_server binary not found${NC}"
        echo "Please build the project first: make all"
        exit 1
    fi
    
    if [ ! -f "./test_client" ]; then
        echo -e "${RED}Error: test_client binary not found${NC}"
        echo "Please build the test client first"
        exit 1
    fi
    
    # Check if test config exists
    if [ ! -f "./test_config.json" ]; then
        echo -e "${YELLOW}Creating test configuration...${NC}"
        create_test_config
    fi
    
    echo -e "${GREEN}Prerequisites check passed${NC}"
}

create_test_config() {
    cat > test_config.json << 'EOF'
{
  "network": {
    "mode": "unicast",
    "incremental_feed_a": {
      "ip": "127.0.0.1",
      "port": 20001
    },
    "incremental_feed_b": {
      "ip": "127.0.0.1",
      "port": 20002
    },
    "snapshot_feed": {
      "ip": "127.0.0.1",
      "port": 20003
    },
    "definition_feed": {
      "ip": "127.0.0.1",
      "port": 20004
    }
  },
  "market_data": {
    "snapshot_interval_seconds": 30,
    "message_batch_size": 3,
    "max_book_depth": 5,
    "incremental_publish_interval_ms": 200
  },
  "channels": [
    {
      "id": 310,
      "name": "CME Globex Equity Futures",
      "description": "E-mini futures for testing"
    }
  ],
  "instruments": [
    {
      "symbol": "ESZ4",
      "security_id": 1,
      "name": "E-mini S&P 500 Dec 2024",
      "underlying": "SPX",
      "exchange": "CME",
      "product_group": "Equity",
      "tick_size": 0.25,
      "multiplier": 50,
      "initial_price": 4500.00,
      "initial_spread": 0.25,
      "price_decimals": 2,
      "contract_size": 50.0,
      "initial_margin": 13200.0,
      "maintenance_margin": 12000.0,
      "maturity_date": "2024-12-20"
    },
    {
      "symbol": "NQZ4",
      "security_id": 5,
      "name": "E-mini NASDAQ-100 Dec 2024",
      "underlying": "NDX",
      "exchange": "CME",
      "product_group": "Equity",
      "tick_size": 0.25,
      "multiplier": 20,
      "initial_price": 15500.00,
      "initial_spread": 0.25,
      "price_decimals": 2,
      "contract_size": 20.0,
      "initial_margin": 17600.0,
      "maintenance_margin": 16000.0,
      "maturity_date": "2024-12-20"
    }
  ],
  "scenario": {
    "type": "normal",
    "volatility": "medium",
    "message_rate": 20,
    "duration_minutes": 1,
    "price_drift": 0.0,
    "spread_multiplier": 1.0,
    "book_depth_target": 3
  }
}
EOF
    echo "Created test_config.json"
}

start_mock_server() {
    echo -e "${YELLOW}Starting CME mock server...${NC}"
    
    # Start server in background
    ./cme_mock_server --config test_config.json --verbose --scenario normal > server_test.log 2>&1 &
    SERVER_PID=$!
    
    echo "Mock server started (PID: $SERVER_PID)"
    
    # Wait for server to initialize
    echo "Waiting for server to initialize..."
    sleep 3
    
    # Check if server is still running
    if ! kill -0 "$SERVER_PID" 2>/dev/null; then
        echo -e "${RED}Error: Mock server failed to start${NC}"
        echo "Server log:"
        cat server_test.log
        exit 1
    fi
    
    # Check if server is listening
    if ! netstat -ln 2>/dev/null | grep -q "${TEST_IP}:${TEST_PORT}"; then
        echo -e "${YELLOW}Warning: Server may not be listening on expected port${NC}"
        echo "Network status:"
        netstat -ln | grep "${TEST_PORT}" || echo "No listeners found on port ${TEST_PORT}"
    fi
    
    echo -e "${GREEN}Mock server started successfully${NC}"
}

start_test_client() {
    echo -e "${YELLOW}Starting test client...${NC}"
    
    # Start client in background, capture output
    ./test_client --ip "$TEST_IP" --port "$TEST_PORT" --verbose > client_test.log 2>&1 &
    CLIENT_PID=$!
    
    echo "Test client started (PID: $CLIENT_PID)"
    
    # Wait a moment for client to start
    sleep 2
    
    # Check if client is still running
    if ! kill -0 "$CLIENT_PID" 2>/dev/null; then
        echo -e "${RED}Error: Test client failed to start${NC}"
        echo "Client log:"
        cat client_test.log
        exit 1
    fi
    
    echo -e "${GREEN}Test client started successfully${NC}"
}

monitor_test() {
    echo -e "${YELLOW}Running integration test for ${TEST_DURATION} seconds...${NC}"
    echo -e "Monitoring packet exchange between server and client..."
    echo ""
    
    local start_time=$(date +%s)
    local packets_received=0
    local last_packet_count=0
    
    # Monitor for the specified duration
    for i in $(seq 1 $TEST_DURATION); do
        echo -n "Progress: [$i/$TEST_DURATION] "
        
        # Check if processes are still running
        if ! kill -0 "$SERVER_PID" 2>/dev/null; then
            echo -e "\n${RED}Server process died during test${NC}"
            return 1
        fi
        
        if ! kill -0 "$CLIENT_PID" 2>/dev/null; then
            echo -e "\n${RED}Client process died during test${NC}"
            return 1
        fi
        
        # Count packets received by checking client log
        if [ -f client_test.log ]; then
            packets_received=$(grep -c "Received Packet #" client_test.log 2>/dev/null || echo "0")
            if [ "$packets_received" -gt "$last_packet_count" ]; then
                echo -e "${GREEN}✓ ${packets_received} packets received${NC}"
                last_packet_count=$packets_received
            else
                echo "waiting for packets..."
            fi
        else
            echo "monitoring..."
        fi
        
        sleep 1
    done
    
    echo ""
    echo -e "${BLUE}Test completed. Final packet count: ${packets_received}${NC}"
    
    # Return success if we received any packets
    [ "$packets_received" -gt 0 ]
}

analyze_results() {
    echo -e "${YELLOW}Analyzing test results...${NC}"
    echo ""
    
    # Server analysis
    echo -e "${BLUE}=== Server Analysis ===${NC}"
    if [ -f server_test.log ]; then
        local sent_count=$(grep -c "Sent batch update" server_test.log 2>/dev/null || echo "0")
        local trades_count=$(grep -c "Generated trade" server_test.log 2>/dev/null || echo "0")
        local error_count=$(grep -c "ERROR" server_test.log 2>/dev/null || echo "0")
        
        echo "Messages sent: $sent_count"
        echo "Trades generated: $trades_count"
        echo "Errors: $error_count"
        
        if [ "$error_count" -gt 0 ]; then
            echo -e "${RED}Server errors detected:${NC}"
            grep "ERROR" server_test.log | head -5
        fi
    else
        echo -e "${RED}No server log found${NC}"
    fi
    
    echo ""
    
    # Client analysis
    echo -e "${BLUE}=== Client Analysis ===${NC}"
    if [ -f client_test.log ]; then
        local received_count=$(grep -c "Received Packet #" client_test.log 2>/dev/null || echo "0")
        local parsed_count=$(grep -c "Packet parsed successfully" client_test.log 2>/dev/null || echo "0")
        local failed_count=$(grep -c "Packet parsing failed" client_test.log 2>/dev/null || echo "0")
        
        echo "Packets received: $received_count"
        echo "Successfully parsed: $parsed_count"
        echo "Parse failures: $failed_count"
        
        if [ "$received_count" -gt 0 ]; then
            # Show a sample of received data
            echo ""
            echo -e "${BLUE}Sample packet data:${NC}"
            grep -A 10 "Received Packet #1" client_test.log | head -15
        fi
        
        if [ "$failed_count" -gt 0 ]; then
            echo -e "${RED}Client parsing errors detected:${NC}"
            grep -A 2 "Packet parsing failed" client_test.log | head -10
        fi
    else
        echo -e "${RED}No client log found${NC}"
    fi
}

print_summary() {
    echo ""
    echo -e "${BLUE}=== Test Summary ===${NC}"
    
    local success=true
    
    # Check if we have log files
    if [ ! -f server_test.log ] || [ ! -f client_test.log ]; then
        echo -e "${RED}✗ Missing log files${NC}"
        success=false
    else
        local packets_received=$(grep -c "Received Packet #" client_test.log 2>/dev/null || echo "0")
        local packets_parsed=$(grep -c "Packet parsed successfully" client_test.log 2>/dev/null || echo "0")
        local parse_failures=$(grep -c "Packet parsing failed" client_test.log 2>/dev/null || echo "0")
        
        if [ "$packets_received" -gt 0 ]; then
            echo -e "${GREEN}✓ Packets received: $packets_received${NC}"
        else
            echo -e "${RED}✗ No packets received${NC}"
            success=false
        fi
        
        if [ "$packets_parsed" -gt 0 ]; then
            echo -e "${GREEN}✓ Packets successfully parsed: $packets_parsed${NC}"
        else
            echo -e "${RED}✗ No packets successfully parsed${NC}"
            success=false
        fi
        
        if [ "$parse_failures" -eq 0 ]; then
            echo -e "${GREEN}✓ No parse failures${NC}"
        else
            echo -e "${YELLOW}⚠ Parse failures: $parse_failures${NC}"
        fi
        
        # Check for proper SBE structure
        if grep -q "Template ID: 46" client_test.log 2>/dev/null && 
           grep -q "Block Length: 11" client_test.log 2>/dev/null; then
            echo -e "${GREEN}✓ Correct SBE message structure (Template ID: 46, Block Length: 11)${NC}"
        else
            echo -e "${RED}✗ Incorrect SBE message structure${NC}"
            success=false
        fi
        
        # Check for order entries (indicating proper CME format)
        if grep -q "Order Entries:" client_test.log 2>/dev/null; then
            echo -e "${GREEN}✓ OrderID entries present (CME production format)${NC}"
        else
            echo -e "${YELLOW}⚠ No OrderID entries detected${NC}"
        fi
    fi
    
    echo ""
    if [ "$success" = true ] && [ "$packets_received" -gt 0 ] && [ "$packets_parsed" -gt 0 ]; then
        echo -e "${GREEN}🎉 INTEGRATION TEST PASSED${NC}"
        echo -e "${GREEN}CME mock server and test client are compatible!${NC}"
        return 0
    else
        echo -e "${RED}❌ INTEGRATION TEST FAILED${NC}"
        echo -e "${RED}Issues detected in server-client compatibility${NC}"
        return 1
    fi
}

# Main execution
main() {
    print_header
    check_prerequisites
    
    start_mock_server
    start_test_client
    
    if monitor_test; then
        analyze_results
        print_summary
    else
        echo -e "${RED}Test monitoring failed${NC}"
        analyze_results
        return 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --duration)
            TEST_DURATION="$2"
            shift 2
            ;;
        --port)
            TEST_PORT="$2"
            shift 2
            ;;
        --ip)
            TEST_IP="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --duration SECONDS  Test duration (default: 10)"
            echo "  --port PORT         Test port (default: 20001)"
            echo "  --ip IP             Test IP (default: 127.0.0.1)"
            echo "  --help              Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Run the main test
main