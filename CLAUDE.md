# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CME Mock MDP Server - A mock Chicago Mercantile Exchange (CME) Market Data Platform (MDP 3.0) server for testing CME gateway clients. This server simulates realistic market data feeds with full order book state management and generates synchronized snapshots and incremental updates using the SBE (Simple Binary Encoding) protocol.

## Building the Project

### CMake Build (Recommended)
```bash
mkdir build && cd build
cmake .. && make -j4
```

### Quick Makefile Build
```bash
make all
```

### Format Code Before Committing
```bash
clang-format --style=WebKit -i src/**/*.cpp include/**/*.h test/*.cpp
```

## Running Tests

### Individual Test Programs
```bash
./test_order_book          # Test order book operations
./test_mdp_encoding        # Test MDP message encoding
./test_reference_data      # Test reference data management
./test_scenarios           # Test market scenarios
./test_udp_publisher       # Test UDP network layer
```

### System Integration Tests
```bash
./scripts/test_system.sh    # Full system test suite
./scripts/test_server.sh    # Server integration test
./scripts/test_end_to_end.sh # End-to-end protocol test
```

### Running the Server
```bash
./cme_mock_server [config_file]  # Default: config/server_config.json
```

### Debug Clients
```bash
./debug_client              # Debug snapshot/incremental feeds
./cme_test_client          # Full CME client test
./list_instruments         # List configured instruments
```

## Architecture

### Core Components

**Order Book Management** (`src/core/`)
- `order_book.cpp/h` - Individual instrument order book with bid/ask levels
- `order_book_manager.cpp/h` - Manages multiple order books and coordinates updates
- `market_data_generator.cpp/h` - Generates realistic market movements
- `reference_data.cpp/h` - Manages instrument definitions and metadata

**Network Layer** (`src/network/`)
- `udp_publisher.cpp/h` - UDP unicast/multicast publisher
- `feed_publisher.cpp/h` - Manages snapshot and incremental feed publishing
- `udp_receiver.cpp/h` - UDP receiver for testing

**Message Encoding** (`src/messages/`)
- `sbe_encoder.cpp/h` - SBE message encoding utilities
- `cme_sbe_encoder.cpp/h` - CME MDP 3.0 specific encoding
- `message_factory.cpp/h` - Creates appropriate message types
- `mdp_messages.h` - MDP message definitions

**Configuration** (`src/config/`)
- `configuration.cpp/h` - JSON configuration parsing
- Server config includes network settings, instruments, and market scenarios

### SBE Schema (`include/cme_sbe/`)
Generated CME MDP 3.0 message definitions including:
- Market data incremental refresh messages (Book, Trade, Volume, Statistics)
- Snapshot messages (Full refresh, Order book)
- Instrument definition messages (Futures, Options, FX, etc.)
- Administrative messages (Login, Heartbeat, Channel reset)

## Key Implementation Details

### MDP 3.0 Protocol
- Uses SBE for binary encoding with schema version 13
- Packet structure: Binary Packet Header + Message Header + Message Body
- Supports multiple messages per packet
- Implements proper sequence numbering and timestamps

### Order Book State
- Maintains synchronized state between snapshots and incremental updates
- Supports configurable book depth (default: 10 levels)
- Handles crossed book detection and resolution
- Tracks market statistics (OHLC, volume, VWAP)

### Feed Publishing
- **Incremental Feed**: Real-time order book updates (default: 10ms intervals)
- **Snapshot Feed**: Periodic full book state (default: 30s intervals)
- **Definition Feed**: Instrument definitions on startup
- Supports both unicast and multicast modes

### Market Scenarios
Configurable scenarios in `server_config.json`:
- Normal trading with medium volatility
- High volatility stress testing
- Trending markets with price drift
- Custom spread multipliers and book depth targets

## Common Development Tasks

### Adding New Instruments
Edit `config/server_config.json` to add instruments with required fields:
- symbol, security_id, tick_size, multiplier
- initial_price, initial_spread, price_decimals
- contract_size, margin requirements

### Testing Specific Messages
Use test programs in `test/` directory:
- `test_packet_structure.cpp` - Verify packet format
- `test_sbe_encoding.cpp` - Test SBE encoding
- `test_multi_message_packet.cpp` - Test multi-message packets

### Debugging Network Issues
```bash
./network_debug            # Network packet analyzer
./simple_hex_capture       # Capture and display hex packets
./debug_client -v          # Verbose client debugging
```

## Important Notes

- The server simulates CME MDP 3.0 protocol but is for testing purposes only
- All generated data is synthetic and not real market data
- Supports 40+ preconfigured instruments (equity futures, FX spot)
- Message sequence numbers and timestamps follow CME specifications
- Binary packet headers include message size fields per CME requirements