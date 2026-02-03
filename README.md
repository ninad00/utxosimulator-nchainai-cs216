# Bitcoin UTXO Simulator

A comprehensive C++ educational simulator for understanding Bitcoin's UTXO (Unspent Transaction Output) model, transaction management, mempool operations, blockchain mining, dynamic fee mechanisms, and block reward halving.


## Features

- **UTXO Management**: Track unspent transaction outputs with owner information and balance calculation
- **Transaction Creation**: Create and validate complex transactions with multiple inputs and outputs
- **Dynamic Fee System**: Configurable percentage-based transaction fees with automatic calculation
- **Mempool with Priority Picking**: Fee-based transaction prioritization with size limits and double-spend prevention
- **Block Mining**: Mine blocks with configurable transaction limits and intelligent transaction selection
- **Block Rewards with Halving**: Configurable block rewards with periodic halving mechanism
- **Blockchain Viewing**: Visual ASCII-art blockchain display with complete transaction history
- **Interactive UI**: User-friendly command-line interface for all operations
- **Comprehensive Test Suite**: 13 built-in test scenarios covering all functionality

## System Architecture

The simulator consists of five main components working together:

### 1. UTXO Manager (`utxo_manager.h`)
Manages the global set of unspent transaction outputs.

**Key Functions:**
- `add_utxo()`: Adds a new UTXO to the set
- `remove_utxo()`: Removes a spent UTXO from the set
- `get_balance()`: Calculates total balance for an owner
- `get_utxos_for_owner()`: Retrieves all UTXOs belonging to a specific owner
- `exists()`: Checks if a UTXO exists in the set
- `display()`: Shows all UTXOs in a formatted table

**Data Structure:**
```cpp
map<pair<string, int>, UTXO> utxo_set;
// Key: (transaction_id, output_index)
// Value: UTXO{tx_id, index, amount, owner}
```

### 2. Transaction (`transaction.h`)
Defines the structure of transactions with inputs and outputs.

**Components:**
- **Input**: References a previous transaction output to spend
  - `prev_tx_id`: ID of the transaction containing the UTXO
  - `index`: Output index in that transaction
  - `owner`: Current owner of the UTXO
- **Output**: Creates new UTXOs
  - `amount`: Amount of Bitcoin
  - `address`: Recipient's address
- **Transaction**: Complete transaction structure
  - `tx_id`: Unique transaction identifier
  - `inputs`: Vector of inputs to spend
  - `outputs`: Vector of outputs to create
  - `fee`: Transaction fee (calculated dynamically)

### 3. Mempool (`mempool.h`)
Manages pending transactions before they are mined into blocks.

**Key Features:**
- **Double-Spend Prevention**: Tracks spent UTXOs to prevent conflicts
- **Transaction Validation**: Comprehensive validation before acceptance
- **Priority-Based Selection**: Orders transactions by fee for mining
- **Size Limiting**: Enforces maximum mempool size with eviction
- **Dynamic Fee Calculation**: Fees calculated as `total_input - total_output`

**Key Functions:**
- `add_transaction()`: Validates and adds transaction to mempool
- `remove_transaction()`: Removes a transaction (after mining)
- `get_top_transactions(n)`: Returns top N transactions by fee
- `clear()`: Empties the mempool
- `display()`: Shows all pending transactions

**Validation Checks:**
1. All input UTXOs exist in the UTXO set
2. No double-spending within the same transaction
3. No UTXOs already spent in other mempool transactions
4. Sufficient funds (inputs ≥ outputs)
5. No negative output amounts

### 4. Block (`block.h`)
Represents blockchain blocks with mining functionality.

**Block Structure:**
```cpp
struct Block {
    int block_height;           // Block number in the chain
    string miner;               // Miner's address
    vector<Transaction> transactions;  // Included transactions
    double total_fees;          // Sum of all transaction fees
    double block_reward;        // Block subsidy (with halving)
    string timestamp;           // Block creation time
};
```

**Mining Process (`mine_block` function):**
1. Select top transactions from mempool based on fees
2. Process each transaction:
   - Remove spent input UTXOs
   - Add new output UTXOs
   - Accumulate transaction fees
   - Remove from mempool
3. Create coinbase transaction for miner
4. Award miner: block_reward + total_fees
5. Timestamp the block
6. Return the newly mined block

### 5. Simulator (`simulator.h`)
Main orchestrator providing the interactive interface.

**Configuration Parameters:**
- `gas_fee`: Percentage fee for transactions (e.g., 0.01 = 1%)
- `block_reward`: Initial block reward in BTC
- `mempool_maxsize`: Maximum transactions in mempool
- `k`: Number of blocks between reward halvings

**Key Functions:**
- `run()`: Main interactive loop
- `create_transaction_ui()`: User interface for creating transactions
- `display_blockchain()`: Visual blockchain display with ASCII art
- `run_test_scenarios()`: Executes comprehensive test suite

## Getting Started

### Prerequisites

- C++ compiler with C++11 support (g++, clang++)
- Standard C++ library

### Compilation

```bash
g++ -std=c++11 -o simulator src/main.cpp
```

Or with optimizations:
```bash
g++ -std=c++11 -O2 -o simulator src/main.cpp
```

The `main.cpp` file initializes the simulator with specific parameters:
```cpp
#include "simulator.h"

int main() {
    // Parameters: gas_fee, block_reward, mempool_maxsize, halving_interval
    Simulator sim(0.01, 6.25, 5, 2);
    sim.run();
    return 0;
}
```

**Parameter Explanation:**
- `0.01`: 1% transaction fee
- `6.25`: Initial block reward (current Bitcoin reward as of 2020)
- `5`: Maximum 5 transactions in mempool
- `2`: Block reward halves every 2 blocks (for testing; real Bitcoin is 210,000)
```

### Running the Simulator

```bash
./simulator
```

## Usage

The simulator provides an interactive menu with the following options:

1. **Create new transaction**: Send Bitcoin between users
2. **View UTXO set**: Display all unspent transaction outputs
3. **View mempool**: Show pending transactions with fees
4. **Mine block**: Mine a block with transactions from the mempool
5. **View blockchain**: Display the complete blockchain with visual representation
6. **Run test scenarios**: Execute 13 comprehensive test cases
7. **Exit**: Quit the simulator

### Initial State

The simulator starts with a genesis block containing UTXOs for five users:
- Alice: 50 BTC
- Bob: 30 BTC
- Charlie: 20 BTC
- David: 10 BTC
- Eve: 5 BTC

Total initial supply: 115 BTC

## Complete Usage Examples

### Example 1: Simple Transaction

```
=== Bitcoin Transaction Simulator ===
Initial UTXOs (Genesis Block):
TX ID                Index     Amount         Owner
genesis              0         50.000         Alice
genesis              1         30.000         Bob
genesis              2         20.000         Charlie

Main Menu:
1. Create new transaction
Enter choice: 1

Enter sender: Alice
Available balance: 50.000 BTC
Enter recipient: Bob
Enter amount: 10

Creating transaction...
Transaction valid! Fee: 0.100
Transaction ID: tx_Alice_Bob_123
Transaction added to mempool.
Mempool now has 1 transactions.
```

### Example 2: View Mempool

```
Main Menu:
3. View mempool
Enter choice: 3

--- Current Mempool ---
TX ID                     Inputs  Outputs  Fee
tx_Alice_Bob_123          1       2        0.100
```

### Example 3: Mine Block

```
Main Menu:
4. Mine block
Enter choice: 4

Enter miner name: Charlie
Mining block...
Selected 1 transactions from mempool.
Block Reward: 6.250 BTC
Total fees: 0.100 BTC
Miner Charlie receives 6.350 BTC
Block mined successfully!
```

### Example 4: View Blockchain

```
Main Menu:
5. View blockchain
Enter choice: 5

+--------------------------------------------------+
| Block #1 | Miner: Charlie                        |
| Time: Mon Feb  3 14:56:04 2026                   |
| Reward: 6.250 | Fees: 0.100 BTC                  |
| TXs: 1                                           |
|   +-- tx_Alice_... [1->2] Fee: 0.100            |
+--------------------------------------------------+
```

### Example 5: Check Updated UTXO Set

```
Main Menu:
2. View UTXO set
Enter choice: 2

--- Current UTXO Set ---
TX ID                Index     Amount         Owner
genesis              1         30.000         Bob
genesis              2         20.000         Charlie
genesis              3         10.000         David
genesis              4         5.000          Eve
tx_Alice_Bob_123     0         10.000         Bob
tx_Alice_Bob_123     1         39.900         Alice
coinbase_block_1     0         6.350          Charlie
```

## File Structure

```
CS216-nchainai-UTXO-Simulator/
├── src/
│   ├── utxo_manager.h    # UTXO set management
│   │   ├── UTXO struct
│   │   ├── UTXOManager class
│   │   └── Balance calculation and UTXO tracking
│   ├── transaction.h     # Transaction data structures
│   │   ├── Input struct (references previous outputs)
│   │   ├── Output struct (creates new UTXOs)
│   │   └── Transaction struct (complete tx definition)
│   ├── mempool.h        # Transaction pool management
│   │   ├── Mempool class
│   │   ├── Transaction validation logic
│   │   ├── Priority-based selection
│   │   └── Double-spend prevention
│   ├── block.h          # Block structure and mining
│   │   ├── Block struct
│   │   ├── mine_block() function
│   │   └── Block display with ASCII art
│   ├── simulator.h      # Main simulator with UI
│   │   ├── Simulator class
│   │   ├── Interactive menu system
│   │   ├── Transaction creation UI
│   │   ├── Blockchain visualization
│   │   └── Test scenario runner
│   └── main.cpp         # Entry point
│       └── Simulator initialization with parameters
├── test/
│   └── tests.h         # Comprehensive test suite
│       └── 13 test scenarios covering all functionality
├── Assignment 1.pdf    # Project assignment document
└── README.md          # This comprehensive documentation
```


## Configuration Guide

### Simulator Parameters

**Full Constructor:**
```cpp
Simulator(double gas_fee, double block_reward, 
          int mempool_maxsize, int halving_interval)
```

**Parameter Details:**

| Parameter | Type | Description | Example | Typical Range |
|-----------|------|-------------|---------|---------------|
| `gas_fee` | double | Transaction fee percentage | 0.01 (1%) | 0.001 - 0.05 |
| `block_reward` | double | Initial block reward in BTC | 6.25 | 1.0 - 50.0 |
| `mempool_maxsize` | int | Max transactions in mempool | 5 | 5 - 100 |
| `halving_interval` | int | Blocks between halvings | 2 | 2 - 210000 |

**Configuration Examples:**

**Test Configuration (Fast Halving):**
```cpp
Simulator sim(0.01, 6.25, 5, 2);
// Quick halving for testing (every 2 blocks)
```

**Bitcoin-like Configuration:**
```cpp
Simulator sim(0.001, 6.25, 100, 210000);
// Simulates real Bitcoin parameters
```

**High Fee Environment:**
```cpp
Simulator sim(0.05, 3.125, 50, 100);
// 5% fees, lower reward, larger mempool
```

### Customizing Genesis Block

To modify initial balances, edit `simulator.h`:
```cpp
Simulator(/* parameters */) {
    // Customize initial distribution
    utxo_manager.add_utxo("genesis", 0, 100.0, "Alice");
    utxo_manager.add_utxo("genesis", 1, 50.0, "Bob");
    utxo_manager.add_utxo("genesis", 2, 25.0, "Charlie");
    // Add more users as needed
}
```

### Adjusting Block Size

Modify the mining transaction limit in `simulator.h`:
```cpp
Block new_block = mine_block(miner, mempool, utxo_manager, 
                             blockchain.size() + 1, block_reward, 10);
                             //                                  ^^
                             // Change this to adjust block capacity
```



## Troubleshooting

### Common Issues

**Issue: "Insufficient funds for fee"**
```
Problem: Total UTXOs < Amount × (1 + gas_fee)
Solution: Check sender's balance includes enough for fee
Example: Sending 10 BTC with 1% fee requires 10.10 BTC total
```

**Issue: "UTXO already spent in mempool"**
```
Problem: Trying to spend UTXO that's in a pending transaction
Solution: Wait for pending transaction to be mined, or use different UTXO
```

**Issue: "Mempool is full"**
```
Problem: Mempool at capacity, transaction has low fee
Solution: Increase transaction fee or wait for block to be mined
Current: Low-fee transactions automatically evicted
```

**Issue: "All available UTXOs are already pending in mempool"**
```
Problem: All sender's UTXOs are locked in mempool transactions
Solution: Wait for transactions to be mined to free up UTXOs
```

**Issue: "No transactions to mine"**
```
Problem: Mempool is empty
Solution: Create transactions before attempting to mine
```



## Performance Considerations

### Computational Complexity

**UTXO Lookup:** O(log n) - Map-based storage
**Balance Calculation:** O(n) - Iterates all UTXOs
**Transaction Validation:** O(m) - Checks m inputs
**Mempool Sorting:** O(n log n) - Sorts by fee
**Block Mining:** O(k) - Processes k transactions

### Memory Usage

**UTXO Set:** Grows with each transaction output
**Mempool:** Limited by max_size parameter
**Blockchain:** Grows linearly with blocks
**Optimization:** Old blocks could be archived in production system






