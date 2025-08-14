# CME Mock MDP Server

A mock CME Market Data Platform (MDP 3.0) server for testing CME gateway clients. Maintains internal order book state and generates consistent snapshots and incremental updates.

## Features

- Full order book state management
- MDP 3.0 protocol support
- Synchronized snapshot and incremental feeds
- Configurable market scenarios
- UDP unicast/multicast support
- Multiple instrument support

## Building

```bash
mkdir build
cd build
cmake ..
make -j4
```

## Running

```bash
./cme_mock_server [config_file]
```

Default config file: `config/server_config.json`

## Configuration

See `config/server_config.json` for example configuration including:
- Network settings (IP addresses, ports)
- Instrument definitions
- Market scenarios
- Publishing intervals

## Architecture

- **Order Book Manager**: Maintains current market state
- **Feed Publishers**: Sends snapshot and incremental updates
- **Market Data Generator**: Creates realistic market movements
- **Configuration System**: JSON-based configuration

## Testing

Connect your CME client to:
- Snapshot feed: 127.0.0.1:14320
- Incremental feed: 127.0.0.1:14310