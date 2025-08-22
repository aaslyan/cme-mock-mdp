# CME Mock Server Test Scripts

This directory contains comprehensive test scripts for validating the CME Mock Server and Test Client integration.

## Available Test Scripts

### 1. Quick Test (`quick_test.sh`)
**Purpose**: Fast validation of basic server-client communication
**Duration**: ~10 seconds
**Usage**:
```bash
./quick_test.sh
```

**What it tests**:
- Server startup with minimal configuration
- Client connection and packet reception
- Basic SBE message parsing
- CME MDP 3.0 format validation (Template ID 46, Block Length 11)

**Expected output**:
```
✓ Quick test PASSED - Basic communication working
✓ Correct CME MDP 3.0 format detected
```

### 2. Full Integration Test (`test_cme_integration.sh`)
**Purpose**: Comprehensive end-to-end testing with detailed analysis
**Duration**: Configurable (default 10 seconds)
**Usage**:
```bash
./test_cme_integration.sh [options]

Options:
  --duration SECONDS  Test duration (default: 10)
  --port PORT         Test port (default: 20001)
  --ip IP             Test IP (default: 127.0.0.1)
  --help              Show help
```

**What it tests**:
- Complete server-client lifecycle
- Packet exchange monitoring
- Detailed packet structure analysis
- Error detection and reporting
- Performance metrics
- SBE message validation
- OrderID entries verification

**Expected output**:
```
🎉 INTEGRATION TEST PASSED
CME mock server and test client are compatible!
```

## Test Validation Criteria

Both tests verify:

1. **Network Communication**
   - Server binds to correct port
   - Client connects successfully
   - UDP packets are transmitted and received

2. **CME MDP 3.0 Protocol Compliance**
   - Correct packet header structure (12 bytes)
   - Proper message size fields (2 bytes each)
   - Valid SBE message headers
   - Template ID 46 (MDIncrementalRefreshBook46)
   - Block Length 11 (CME production format)

3. **Message Content**
   - NoMDEntries group with price levels
   - NoOrderIDEntries group (CME production feature)
   - Proper 8-byte alignment
   - Valid field encoding

4. **Client Compatibility**
   - Packets can be parsed without errors
   - Message advancement works correctly
   - SBE decoders function properly

## Troubleshooting

### Test Fails: "No packets received"
- Check if server is using correct configuration
- Verify network connectivity
- Ensure no firewall blocking localhost traffic
- Check server logs for binding errors

### Test Fails: "Packet parsing failed"  
- Verify SBE message format
- Check 8-byte alignment implementation
- Validate message size fields
- Review packet structure

### Server Won't Start
- Ensure binaries are built: `make all`
- Check configuration file syntax
- Verify required fields are present
- Review server logs for specific errors

## Manual Testing

For manual testing, run components separately:

**Start Server**:
```bash
./cme_mock_server --config test_config.json --verbose
```

**Start Client** (in another terminal):
```bash
./test_client --ip 127.0.0.1 --port 20001 --verbose
```

## Expected Packet Flow

1. **Server Startup**: Initializes order books, binds to UDP port
2. **Client Startup**: Binds to UDP port, starts listening
3. **Market Data**: Server generates price updates every 200-500ms
4. **Packet Transmission**: Server sends CME-formatted UDP packets
5. **Client Processing**: Client receives and parses packets
6. **Validation**: SBE messages decoded successfully

## Success Indicators

- ✅ Packets received > 0
- ✅ Packets parsed successfully > 0  
- ✅ Template ID = 46
- ✅ Block Length = 11
- ✅ OrderID entries present
- ✅ No parsing errors
- ✅ Proper message advancement

## Performance Expectations

- **Packet Rate**: 5-20 packets/second (configurable)
- **Packet Size**: ~88-132 bytes (typical CME size)
- **Latency**: < 1ms for localhost communication
- **Success Rate**: 100% packet parsing success