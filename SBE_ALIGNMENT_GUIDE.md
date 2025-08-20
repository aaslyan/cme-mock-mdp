# SBE Encoding/Decoding Alignment Guide

## Overview
This guide ensures compatibility between the CME Mock Server's SBE encoding and your client's SBE decoding.

## Key Alignment Points

### 1. Schema Version
- **Schema ID**: 1 (CME MDP 3.0)
- **Schema Version**: 13
- **Byte Order**: Little-Endian
- Both server and client MUST use the same schema version

### 2. Message Structure

#### Binary Packet Header (12 bytes)
```
Offset 0-3:  Sequence Number (uint32, little-endian)
Offset 4-11: Sending Time (uint64, little-endian)
```

#### Message Size Field (2 bytes)
```
Offset 12-13: Message Size (uint16, little-endian)
              Size of the following SBE message
```

#### SBE Message Header (8 bytes)
```
Offset 0-1: Block Length (uint16, little-endian)
Offset 2-3: Template ID (uint16, little-endian)
Offset 4-5: Schema ID (uint16, little-endian)
Offset 6-7: Version (uint16, little-endian)
```

### 3. Common Message Types

#### MDIncrementalRefreshBook46
- Template ID: 46
- Block Length: 11 bytes
- Fixed Fields:
  - TransactTime (uint64) at offset 0
  - MatchEventIndicator (uint8) at offset 8
  - Padding (2 bytes) at offset 9-10

#### SnapshotFullRefresh52
- Template ID: 52
- Block Length: 59 bytes
- Contains market data snapshot entries

### 4. Repeating Groups
- Group header: 4 bytes
  - Block Length (uint16)
  - Num In Group (uint16)
- Each entry follows immediately after header
- No padding between entries

### 5. Field Types and Encoding

#### PRICE9 Composite
- Mantissa: int64 (8 bytes)
- Exponent: constant -9
- Example: 285.00 = mantissa 2850000000000

#### Common Field Types
- SecurityID: uint32
- MDEntrySize: uint32
- NumberOfOrders: uint32
- MDPriceLevel: uint8
- MDUpdateAction: uint8 enum
- MDEntryType: char enum

## Validation Tools

### 1. test_sbe_compatibility
Generates reference SBE messages with hex dumps for verification.

```bash
./test_sbe_compatibility
```

### 2. validate_sbe_alignment
Creates binary reference packets and analyzes message structure.

```bash
./validate_sbe_alignment
```

Output: `reference_packet.bin` - Binary file for client testing

### 3. Hex Dump Verification
Compare server-generated hex dumps with client interpretation:

Example MDIncrementalRefreshBook46 packet:
```
0000: 78 56 34 12 88 77 66 55 44 33 22 11 36 00 0b 00  // Packet + Msg Size
0010: 2e 00 01 00 0d 00 11 00 ff ee dd cc bb aa 87 00  // Header + TransactTime
0020: 00 20 00 01 00 d4 3c 91 97 02 00 00 64 00 00 00  // Group + Entry
```

## Client Verification Checklist

1. **Endianness**: Verify little-endian byte order
2. **Header Parsing**: Check 8-byte SBE header structure
3. **Template IDs**: Match message types (46, 52, etc.)
4. **Field Offsets**: Validate field positions match specification
5. **Group Handling**: Correct repeating group parsing
6. **PRICE9 Decoding**: Proper mantissa/exponent handling

## Common Issues and Solutions

### Issue: Misaligned Fields
**Solution**: Check for padding bytes in message structure. The server includes 2-byte padding after MatchEventIndicator in MDIncrementalRefreshBook46.

### Issue: Wrong Values
**Solution**: Verify endianness. All multi-byte values are little-endian.

### Issue: Group Count Mismatch
**Solution**: Group count is uint16 (2 bytes), not uint8.

### Issue: Message Size Confusion
**Solution**: Message size field (bytes 12-13) indicates size of SBE message only, not including packet header.

## Testing Procedure

1. Run validation tools to generate reference data
2. Feed `reference_packet.bin` to your client
3. Compare decoded values with expected:
   - Sequence: 0x12345678
   - TransactTime: 0xAABBCCDDEEFF0011
   - SecurityID: 123456 (0x0001E240)
   - Price: 285.00 (mantissa: 2850000000000)

## Schema Files

The server uses generated SBE headers from CME's official schema:
- Location: `include/cme_sbe/`
- Key headers: MessageHeader.h, MDIncrementalRefreshBook46.h, SnapshotFullRefresh52.h

Ensure your client uses the same schema version or compatible definitions.