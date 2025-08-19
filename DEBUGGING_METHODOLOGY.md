# CME MDP 3.0 Binary Alignment Debugging Methodology

## Problem Summary

The CME Mock Server was sending binary SBE data with incorrect packet structure alignment, causing the CME gateway to reject messages. The issue was in the Binary Packet Header format.

## Root Cause Analysis

### Issue Identified
1. **Missing Message Count Field**: The Binary Packet Header was only 12 bytes instead of the required 14 bytes
2. **Incorrect Field Positioning**: Missing the 2-byte Message Count field at offset 12-13

### Correct CME MDP 3.0 Binary Packet Structure

```
Binary Packet Header (14 bytes):
┌─────────────────┬─────────────────┬──────────────────┐
│ Sequence Number │  Sending Time   │ Message Count    │
│    (4 bytes)    │    (8 bytes)    │   (2 bytes)      │
│   Offset 0-3    │   Offset 4-11   │  Offset 12-13    │
└─────────────────┴─────────────────┴──────────────────┘

For each message:
┌─────────────────┬─────────────────┬──────────────────┐
│ Message Size    │ SBE Msg Header  │ SBE Message Body │
│   (2 bytes)     │    (8 bytes)    │   (variable)     │
└─────────────────┴─────────────────┴──────────────────┘
```

## Debugging Tools Created

### 1. Packet Analyzer (`debug_packet_analyzer.cpp`)
- Analyzes binary packet structure
- Compares against CME MDP 3.0 specification  
- Shows hex dumps with field boundaries
- Validates SBE header format

### 2. Fixed Structure Test (`test_fixed_packet_structure.cpp`)
- Verifies corrected packet format
- Tests Binary Packet Header encoding
- Validates SBE message structure
- Confirms complete packet assembly

## Solution Implemented

### Code Changes Made

1. **Fixed `CMESBEEncoder::encode_packet_header()`**:
   ```cpp
   // Added msg_count parameter
   static std::vector<uint8_t> encode_packet_header(
       uint32_t sequence_number,
       uint64_t sending_time,
       uint16_t msg_count = 1);
   ```

2. **Fixed `MDPMessageEncoder::encode_packet_header()`**:
   ```cpp
   // Added message count field at correct offset
   encoder.encode_uint32(sequence_number);  // Offset 0-3
   encoder.encode_uint64(sending_time);     // Offset 4-11  
   encoder.encode_uint16(msg_count);        // Offset 12-13
   ```

3. **Updated `feed_publisher.cpp`**:
   ```cpp
   auto packet_header = MDPMessageEncoder::encode_packet_header(
       snapshot.header.sequence_number,
       snapshot.header.sending_time,
       snapshot.header.msg_count);  // Now includes msg_count
   ```

## Verification Steps

### Test Results
- ✅ Binary Packet Header: 14 bytes with correct field layout
- ✅ SBE Message Header: Template ID 52, Schema ID 1, Version 13
- ✅ Message Size Field: Correctly placed before each SBE message
- ✅ Complete packet structure matches CME specification

### Debug Output Example
```
Binary Packet Header (14 bytes):
39 30 00 00 15 81 e9 7d f4 10 22 11 01 00

Parsed:
- Sequence Number: 12345 ✓
- Sending Time: 1234567890123456789 ✓  
- Message Count: 1 ✓
```

## Future Debugging Recommendations

### For Binary Protocol Issues:
1. **Always verify packet structure first** using hex dump tools
2. **Check field alignment** against official protocol documentation
3. **Use the debugging tools** provided in this repository:
   - `./debug_packet_analyzer` - for structure analysis
   - `./test_fixed_packet_structure` - for validation

### For SBE Message Issues:
1. **Verify schema version** matches gateway expectations (13 for FX, 9 for equity)
2. **Check template IDs** are correct for message type
3. **Validate message size fields** precede each SBE message

### Key Debug Commands:
```bash
# Test packet structure
./test_fixed_packet_structure

# Analyze raw packets  
./debug_packet_analyzer

# Test server with debug output
./cme_mock_server config/server_config.json

# Debug client to capture packets
./debug_client -v
```

## Schema Alignment Notes

The generated SBE headers in `include/cme_sbe/` contain the definitive field layouts. Key constants:

```cpp
// From SnapshotFullRefresh52.h
SBE_TEMPLATE_ID = 52
SBE_SCHEMA_ID = 1  
SBE_SCHEMA_VERSION = 13
SBE_BLOCK_LENGTH = 59

// From MessageHeader.h  
encodedLength() = 8 // SBE header is always 8 bytes
```

## Conclusion

The alignment issue was resolved by ensuring the Binary Packet Header includes all required fields in the correct order and offsets as specified by CME MDP 3.0. The debugging methodology provides tools and processes to quickly identify and resolve similar issues in the future.