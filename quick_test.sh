#!/bin/bash

# Quick CME Test Script
# Simple test to verify basic server-client communication

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}Quick CME Integration Test${NC}"
echo "=========================="

# Check if binaries exist
if [ ! -f "./cme_mock_server" ] || [ ! -f "./test_client" ]; then
    echo -e "${RED}Error: Binaries not found. Please build first: make all${NC}"
    exit 1
fi

# Always create a fresh test config
echo "Creating test config..."
cat > quick_test_config.json << 'EOF'
{
  "network": {
    "mode": "unicast",
    "incremental_feed_a": { "ip": "127.0.0.1", "port": 20001 },
    "incremental_feed_b": { "ip": "127.0.0.1", "port": 20002 },
    "snapshot_feed": { "ip": "127.0.0.1", "port": 20003 },
    "definition_feed": { "ip": "127.0.0.1", "port": 20004 }
  },
  "market_data": {
    "snapshot_interval_seconds": 30,
    "message_batch_size": 2,
    "max_book_depth": 3,
    "incremental_publish_interval_ms": 300
  },
  "channels": [
    { "id": 310, "name": "Test Channel", "description": "Test" }
  ],
  "instruments": [
    {
      "symbol": "ESZ4", "security_id": 1, "name": "E-mini S&P 500",
      "underlying": "SPX", "exchange": "CME", "product_group": "Equity",
      "initial_price": 4500.00, "tick_size": 0.25, "multiplier": 50, 
      "initial_spread": 0.25, "price_decimals": 2, "contract_size": 50.0,
      "initial_margin": 13200.0, "maintenance_margin": 12000.0,
      "maturity_date": "2024-12-20"
    }
  ],
  "scenario": {
    "type": "normal", "volatility": "medium", "message_rate": 10,
    "duration_minutes": 1, "price_drift": 0.0, "spread_multiplier": 1.0,
    "book_depth_target": 3
  }
}
EOF

echo "Starting mock server..."
./cme_mock_server --config quick_test_config.json --verbose > /tmp/server.log 2>&1 &
SERVER_PID=$!

# Wait for server startup
echo "Waiting for server startup..."
sleep 3

# Check if server is running
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo -e "${RED}Server failed to start${NC}"
    echo "Server log:"
    cat /tmp/server.log
    exit 1
fi

echo "Starting test client for 6 seconds..."
timeout 6s ./test_client --ip 127.0.0.1 --port 20001 > /tmp/client.log 2>&1 || true

# Check results
PACKETS=$(grep -c "Received Packet" /tmp/client.log 2>/dev/null || echo "0")
PARSED=$(grep -c "parsed successfully" /tmp/client.log 2>/dev/null || echo "0")

echo ""
echo "Results:"
echo "  Packets received: $PACKETS"
echo "  Successfully parsed: $PARSED"

# Check for specific CME message details
TEMPLATE_46=$(grep -c "Template ID: 46" /tmp/client.log 2>/dev/null || echo "0")
BLOCK_11=$(grep -c "Block Length: 11" /tmp/client.log 2>/dev/null || echo "0")

if [ "$PACKETS" -gt 0 ] && [ "$PARSED" -gt 0 ]; then
    echo -e "${GREEN}✓ Quick test PASSED - Basic communication working${NC}"
    
    if [ "$TEMPLATE_46" -gt 0 ] && [ "$BLOCK_11" -gt 0 ]; then
        echo -e "${GREEN}✓ Correct CME MDP 3.0 format detected${NC}"
    fi
    
    # Show sample data
    echo ""
    echo "Sample packet details:"
    grep -A 8 "SBE Header:" /tmp/client.log | head -12 2>/dev/null || echo "No detailed SBE info"
else
    echo -e "${RED}✗ Quick test FAILED${NC}"
    
    # Show debug info
    echo ""
    echo "Debugging info:"
    echo "Server log (last 15 lines):"
    tail -15 /tmp/server.log 2>/dev/null || echo "No server log"
    
    echo ""
    echo "Client log:"
    cat /tmp/client.log 2>/dev/null || echo "No client log"
    
    echo ""
    echo "Network status:"
    netstat -ln | grep 20001 || echo "No listener on port 20001"
fi

# Cleanup
echo ""
echo "Cleaning up..."
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true
rm -f quick_test_config.json

echo "Test completed."