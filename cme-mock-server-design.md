# CME Mock MDP Server - Architecture & Design

## Overview

This document outlines the architecture for a mock CME MDP (Market Data Platform) server that maintains internal order book state and generates consistent snapshots and incremental updates for testing CME gateway clients.

## Architecture Pattern: State Machine with Order Book Management

### Core Design Principles

1. **Stateful Order Book Management** - Maintain accurate order book state internally
2. **Synchronized Data Feeds** - Ensure snapshots and incrementals are always consistent
3. **Deterministic Testing** - Support reproducible test scenarios
4. **Protocol Compliance** - Follow CME MDP 3.0 specifications

## System Architecture

### Component Overview

```
┌─────────────────────────────────────────────────────────┐
│                   Mock MDP Server                        │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────────┐        ┌────────────────────┐    │
│  │ Reference Data   │───────▶│  Order Book        │    │
│  │   Manager        │        │  State Engine      │    │
│  └──────────────────┘        └────────────────────┘    │
│           │                           │                  │
│           ▼                           ▼                  │
│  ┌──────────────────┐        ┌────────────────────┐    │
│  │  Instrument      │        │  Market Data       │    │
│  │  Definitions     │        │  Generator         │    │
│  └──────────────────┘        └────────────────────┘    │
│                                       │                  │
│                    ┌──────────────────┴──────────┐      │
│                    ▼                              ▼      │
│         ┌──────────────────┐          ┌──────────────┐ │
│         │ Snapshot Feed    │          │ Incremental  │ │
│         │  Publisher       │          │ Feed Publisher│ │
│         └──────────────────┘          └──────────────┘ │
│                    │                           │         │
└────────────────────┼───────────────────────────┼────────┘
                     ▼                           ▼
              UDP Multicast               UDP Multicast
             (or Unicast)                (or Unicast)
```

### Core Components

#### 1. Reference Data Manager
**Purpose**: Manages instrument definitions and trading session parameters

**Responsibilities**:
- Load and parse instrument definition files
- Provide security metadata (tick size, multiplier, trading hours)
- Manage product complex relationships
- Handle trading calendar and session schedules

**Data Structure**:
- Security definitions (Symbol, SecurityID, Exchange, Asset Class)
- Contract specifications (tick size, point value, delivery months)
- Trading session parameters (open/close times, trading states)
- Price band limits

#### 2. Order Book State Engine
**Purpose**: Maintains the current state of all order books

**Responsibilities**:
- Maintain bid/ask price levels for each instrument
- Process order additions, modifications, cancellations
- Track trade executions and update last trade price
- Calculate implied prices for spreads (optional)
- Maintain market statistics (high, low, volume, VWAP)

**State Management**:
- Order book depth (10 levels per side minimum)
- Order priority within price levels
- Aggregate quantity at each price level
- Last trade information
- Session statistics

#### 3. Market Data Generator
**Purpose**: Creates realistic market data scenarios

**Responsibilities**:
- Generate price movements based on configured volatility
- Create order flow patterns (normal, fast market, auction)
- Simulate market maker behavior
- Generate trades based on order book crossing
- Handle market state transitions

**Scenarios Supported**:
- Steady state markets with normal volatility
- Fast markets with rapid price changes
- Thin/thick order books
- Opening/closing auctions
- Trading halts and resumptions

#### 4. Snapshot Feed Publisher
**Purpose**: Publishes full market state snapshots

**Responsibilities**:
- Generate MDP 3.0 snapshot messages
- Include full order book depth
- Publish at configurable intervals (e.g., every 30 seconds)
- Assign snapshot sequence numbers
- Handle snapshot request/response (if implemented)

#### 5. Incremental Feed Publisher
**Purpose**: Publishes real-time market updates

**Responsibilities**:
- Generate MDP 3.0 incremental refresh messages
- Maintain packet sequence numbers
- Bundle multiple updates per packet (batching)
- Implement A/B feed redundancy
- Control message rate and timing

## Data Flow Architecture

### Startup Sequence
```
1. Load Configuration
   └─> Network settings (multicast groups, ports)
   └─> Instrument definitions
   └─> Test scenario parameters

2. Initialize State
   └─> Create order books for all instruments
   └─> Set initial prices and spreads
   └─> Initialize sequence numbers

3. Start Publishers
   └─> Begin snapshot publishing cycle
   └─> Start incremental update stream
   └─> Activate market data generation

4. Client Connection Flow
   └─> Client joins multicast groups
   └─> Receives current snapshot
   └─> Begins processing incrementals
   └─> Maintains synchronized state
```

### Message Sequencing Strategy

**Sequence Number Management**:
- Global sequence counter for incremental feed
- Separate sequence for each snapshot cycle
- Sequence number persistence for recovery scenarios
- Gap detection and recovery testing support

**Synchronization Points**:
- Snapshot contains LastMsgSeqNumProcessed
- Incrementals continue from snapshot endpoint
- Client can determine exact join point
- Support for intentional gaps (testing)

## Testing Scenarios

### 1. Basic Connectivity Tests

**Scenario 1.1: Clean Startup**
- Client connects to feeds
- Receives instrument definitions
- Gets initial snapshot
- Processes incrementals successfully

**Scenario 1.2: Late Join**
- Market data already flowing
- Client joins mid-session
- Must recover via snapshot
- Seamlessly continues with incrementals

### 2. Recovery and Resilience Tests

**Scenario 2.1: Packet Loss Simulation**
- Introduce controlled gaps in sequence numbers
- Client detects gaps
- Requests snapshot for recovery
- Resynchronizes successfully

**Scenario 2.2: Feed Failover**
- Primary feed (A) stops
- Client detects failure
- Switches to backup feed (B)
- Continues without data loss

**Scenario 2.3: Network Interruption**
- Complete feed disconnection
- Variable duration (5 seconds to 5 minutes)
- Client reconnection and recovery
- State consistency validation

### 3. Market Condition Tests

**Scenario 3.1: Normal Market**
- Steady price movements
- Regular order flow
- Typical spread widths
- Normal message rates (50-100 msg/sec)

**Scenario 3.2: Fast Market**
- Rapid price changes
- High order volume
- Frequent trade executions
- High message rates (1000+ msg/sec)

**Scenario 3.3: Thin Market**
- Wide spreads
- Few price levels
- Sporadic trading
- Low message rates (5-10 msg/sec)

**Scenario 3.4: Locked/Crossed Market**
- Bid equals or exceeds ask
- Tests client's handling of unusual states
- Resolution through trades

### 4. Session Transition Tests

**Scenario 4.1: Pre-Open**
- No trading, orders accumulate
- Indicative opening price calculations
- Imbalance messages

**Scenario 4.2: Opening Auction**
- Transition from pre-open to continuous
- Opening trade and price establishment
- Order book initialization

**Scenario 4.3: Closing Auction**
- Transition to closing state
- Final trade price determination
- Settlement price publication

**Scenario 4.4: Trading Halt**
- Sudden halt message
- Order book suspension
- Resume trading sequence

### 5. Data Integrity Tests

**Scenario 5.1: Order Book Consistency**
- Verify bid/ask price ordering
- Validate quantity aggregation
- Check price level gaps
- Ensure FIFO ordering within levels

**Scenario 5.2: Trade Validation**
- Trades occur at valid prices
- Volume accumulation accuracy
- High/low price tracking
- VWAP calculations

**Scenario 5.3: Statistics Accuracy**
- Open/high/low/close values
- Volume and trade count
- Price change calculations
- Settlement price updates

### 6. Performance and Stress Tests

**Scenario 6.1: Message Burst**
- Sudden spike to 10,000 msg/sec
- Client buffer management
- Recovery from overload

**Scenario 6.2: Sustained High Load**
- Continuous 5,000 msg/sec for extended period
- Memory management validation
- Processing latency measurement

**Scenario 6.3: Large Order Book**
- 100+ price levels per side
- Snapshot size handling
- Incremental update efficiency

## Configuration Architecture

### Server Configuration

```yaml
# Network Configuration
network:
  mode: unicast  # or multicast
  incremental_feed_a:
    ip: 127.0.0.1
    port: 14310
  incremental_feed_b:
    ip: 127.0.0.1
    port: 14311
  snapshot_feed:
    ip: 127.0.0.1
    port: 14320
  definition_feed:
    ip: 127.0.0.1
    port: 14330

# Market Data Configuration  
market_data:
  snapshot_interval_seconds: 30
  message_batch_size: 10
  max_book_depth: 10
  
# Test Scenario Configuration
scenario:
  type: normal_market  # fast_market, thin_market, etc.
  volatility: medium
  message_rate: 100
  duration_minutes: 60
```

### Instrument Configuration

```yaml
instruments:
  - symbol: ES
    security_id: 1
    name: "E-mini S&P 500"
    tick_size: 0.25
    multiplier: 50
    initial_price: 4500.00
    initial_spread: 0.25
    
  - symbol: GE
    security_id: 2
    name: "Eurodollar"
    tick_size: 0.0025
    multiplier: 2500
    initial_price: 95.00
    initial_spread: 0.0025
```

## Implementation Phases

### Phase 1: Core Infrastructure
- Basic UDP publisher (unicast mode)
- Simple order book implementation
- Manual market updates
- Basic snapshot/incremental messages

### Phase 2: State Management
- Full order book state engine
- Automated market data generation
- Synchronized snapshots and incrementals
- Sequence number management

### Phase 3: Protocol Compliance
- Complete MDP 3.0 message types
- SBE encoding implementation
- Instrument definition messages
- Market state messages

### Phase 4: Advanced Testing
- All test scenarios implementation
- Performance optimizations
- Feed redundancy (A/B feeds)
- Recovery mechanisms

### Phase 5: Tooling and Monitoring
- Admin interface for scenario control
- Real-time monitoring dashboard
- Test result validation
- Performance metrics collection

## Success Criteria

1. **Functional Requirements**
   - Client successfully connects and receives data
   - Order book built correctly from snapshots
   - Incremental updates applied accurately
   - Recovery from disconnection works

2. **Performance Requirements**
   - Support 1000+ messages per second
   - Latency < 1ms for message generation
   - Handle 100+ simultaneous instruments
   - Stable operation for 8+ hours

3. **Testing Requirements**
   - All defined scenarios executable
   - Reproducible test results
   - Automated validation possible
   - Clear error detection and reporting

## Next Steps

1. **Technology Selection**
   - Choose implementation language (C++, Java, Python)
   - Select SBE library or implementation
   - Decide on configuration framework

2. **Development Environment**
   - Set up build system
   - Configure development network
   - Prepare test client setup

3. **Initial Implementation**
   - Start with Phase 1 core components
   - Build simple order book
   - Implement basic UDP publishing
   - Create minimal test client

4. **Validation Approach**
   - Define success metrics
   - Create validation tools
   - Establish testing procedures
   - Document expected behaviors