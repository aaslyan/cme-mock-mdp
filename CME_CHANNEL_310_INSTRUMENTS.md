# CME Channel 310 - Equity Index Futures Instruments

## Overview
This configuration includes all major CME equity index futures products typically found on Channel 310, representing the complete CME Globex equity futures product suite.

## Instruments Included

### E-mini Futures (Standard Size)
| Symbol | Name | Underlying | Contract Size | Tick Size | Tick Value | Initial Margin |
|--------|------|------------|---------------|-----------|------------|----------------|
| ES | E-mini S&P 500 | SPX | $50/point | 0.25 | $12.50 | $13,200 |
| NQ | E-mini NASDAQ-100 | NDX | $20/point | 0.25 | $5.00 | $17,600 |
| YM | E-mini Dow Jones | DJIA | $5/point | 1.0 | $5.00 | $9,800 |
| RTY | E-mini Russell 2000 | RUT | $50/point | 0.10 | $5.00 | $7,200 |
| EMD | E-mini S&P MidCap 400 | MID | $100/point | 0.10 | $10.00 | $12,000 |

### Micro E-mini Futures (1/10th Size)
| Symbol | Name | Underlying | Contract Size | Tick Size | Tick Value | Initial Margin |
|--------|------|------------|---------------|-----------|------------|----------------|
| MES | Micro E-mini S&P 500 | SPX | $5/point | 0.25 | $1.25 | $1,380 |
| MNQ | Micro E-mini NASDAQ-100 | NDX | $2/point | 0.25 | $0.50 | $1,760 |
| MYM | Micro E-mini Dow Jones | DJIA | $0.50/point | 1.0 | $0.50 | $980 |
| M2K | Micro E-mini Russell 2000 | RUT | $5/point | 0.10 | $0.50 | $720 |

### Volatility Products
| Symbol | Name | Underlying | Contract Size | Tick Size | Tick Value | Initial Margin |
|--------|------|------------|---------------|-----------|------------|----------------|
| VX | CBOE Volatility Index | VIX | $1000/point | 0.05 | $50.00 | $4,000 |

## Configuration Files

### JSON Configuration (`config/server_config.json`)
Complete instrument specifications are defined in the JSON configuration including:
- Basic instrument info (symbol, name, security_id)
- Trading parameters (tick_size, multiplier, margins)
- Price formatting (decimals, initial_price, initial_spread)
- Contract specifications (underlying, exchange, maturity)

### Runtime Usage
```bash
# Start server with Channel 310 configuration
./cme_mock_server --config config/server_config.json

# Run with different market scenarios
./cme_mock_server --config config/server_config.json --scenario fast
./cme_mock_server --config config/server_config.json --scenario thin
./cme_mock_server --config config/server_config.json --scenario recovery

# List all instruments
./list_instruments
```

## Channel Information
- **Channel ID**: 310
- **Channel Name**: CME Globex Equity Futures  
- **Description**: E-mini and Micro E-mini equity index futures and volatility products
- **Exchange**: CME/CBOT
- **Product Group**: Equity Index Futures

## Key Features
- **Complete product coverage**: All major US equity index futures
- **Size variations**: Both standard E-mini and Micro E-mini versions
- **Realistic pricing**: Current market-representative initial prices
- **Proper margins**: CME-accurate initial and maintenance margins
- **Volatility products**: VIX futures for volatility trading

## Market Data Feeds
- **Snapshot Feed**: 127.0.0.1:14320 (30-second intervals)
- **Incremental Feed A**: 127.0.0.1:14310 (real-time updates)
- **Incremental Feed B**: 127.0.0.1:14311 (backup feed)
- **Definition Feed**: 127.0.0.1:14330 (instrument definitions)

This configuration provides a comprehensive mock environment for testing CME equity futures market data applications with authentic Channel 310 instrument specifications.