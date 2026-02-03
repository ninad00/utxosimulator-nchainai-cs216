# Bitcoin UTXO Simulator

A C++ educational simulator for understanding Bitcoin's UTXO (Unspent Transaction Output) model, transaction management, mempool operations, and blockchain mining.

## Overview

This project implements a simplified Bitcoin transaction simulator that demonstrates how the UTXO model works in cryptocurrency systems. It provides an interactive command-line interface for creating transactions, managing a mempool, mining blocks, and maintaining a blockchain.

## Features

- **UTXO Management**: Track unspent transaction outputs with owner information
- **Transaction Creation**: Create and validate transactions with multiple inputs and outputs
- **Mempool**: Priority-based transaction pool with fee-based ordering
- **Block Mining**: Mine blocks with transaction selection and reward distribution
- **Blockchain**: Maintain a complete blockchain history
- **Interactive UI**: User-friendly command-line interface for all operations
- **Test Scenarios**: Built-in test cases to demonstrate functionality

## System Architecture

The simulator consists of five main components:

1. **UTXO Manager** (`utxo_manager.h`): Manages the set of unspent transaction outputs
2. **Transaction** (`transaction.h`): Defines transaction structure with inputs and outputs
3. **Mempool** (`mempool.h`): Manages pending transactions with validation and prioritization
4. **Block** (`block.h`): Represents blockchain blocks with mining functionality
5. **Simulator** (`simulator.h`): Main orchestrator with interactive UI

## Getting Started

### Prerequisites

- C++ compiler with C++11 support (g++, clang++)
- Standard C++ library

### Compilation

```bash
g++ -std=c++11 -o simulator main.cpp
```

Note: You'll need to create a `main.cpp` file that includes `simulator.h` and calls the `run()` method:

```cpp
#include "src/simulator.h"

int main() {
    Simulator sim(0.01, 6.25, 50, 2);  // 1% gas fee, 6.25 BTC initial block reward, max 50 transactions in mempool, reward halves after every 2 blocks(epoch)
    sim.run();
    return 0;
}
```

### Running the Simulator

```bash
./simulator
```

## Usage

The simulator provides an interactive menu with the following options:

1. **Create new transaction**: Send Bitcoin between users
2. **View UTXO set**: Display all unspent transaction outputs
3. **View mempool**: Show pending transactions
4. **Mine block**: Mine a block with transactions from the mempool
5. **View blockchain**: Display the complete blockchain
6. **Run test scenarios**: Execute predefined test cases
7. **Exit**: Quit the simulator

### Initial State

The simulator starts with a genesis block containing UTXOs for five users:
- Alice: 50 BTC
- Bob: 30 BTC
- Charlie: 20 BTC
- David: 10 BTC
- Eve: 5 BTC

### Creating a Transaction

When creating a transaction, you'll need to:
1. Enter the sender's name
2. View their available balance
3. Enter the recipient's name
4. Enter the amount to send

The system automatically:
- Selects appropriate UTXOs to cover the amount
- Calculates and applies transaction fees
- Creates change outputs when necessary
- Validates the transaction before adding it to the mempool

### Mining a Block

When mining a block:
1. Enter the miner's name
2. The system selects up to 5 transactions with the highest fees
3. Processes transactions by updating the UTXO set
4. Awards the miner the block reward (6.25 BTC) plus transaction fees
5. Adds the block to the blockchain

## File Structure

```
CS216-nchainai-UTXO-Simulator/
├── src/
│   ├── utxo_manager.h    # UTXO set management
│   ├── transaction.h     # Transaction data structures
│   ├── mempool.h        # Transaction pool management
│   ├── block.h          # Block structure and mining
│   └── simulator.h      # Main simulator with UI
├── Assignment 1.pdf     # Project assignment document
└── README.md           # This file
```

## Key Concepts Demonstrated

### UTXO Model
- Each transaction consumes existing UTXOs as inputs
- Creates new UTXOs as outputs
- Change is returned to the sender as a new UTXO

### Transaction Validation
- Verifies input UTXOs exist in the UTXO set
- Prevents double-spending within the same transaction
- Checks for UTXOs already spent in the mempool
- Validates sufficient funds (inputs >= outputs + fees)
- Rejects negative output amounts

### Mempool Management
- Maintains pending transactions before mining
- Tracks spent UTXOs to prevent double-spending
- Prioritizes transactions by fee for mining
- Enforces maximum size limit
- Validates transactions before acceptance

### Block Mining
- Selects highest-fee transactions from mempool
- Updates UTXO set (removes inputs, adds outputs)
- Awards block reward + transaction fees to miner
- Creates coinbase transaction for miner reward
- Maintains blockchain with timestamps

## Transaction Fees

The simulator supports configurable transaction fees:
- Fee percentage is set during simulator initialization (e.g., 0.01 for 1%)
- Total transaction cost = amount + (amount × fee percentage), or amount × (1 + fee percentage)
- Example: Sending 10 BTC with 1% fee costs 10.10 BTC total
- Miners receive all transaction fees from mined blocks
- Fees incentivize miners to include transactions in blocks

## Example Workflow

1. Start the simulator
2. View initial UTXO set (genesis block)
3. Create a transaction: Alice sends 10 BTC to Bob
4. View mempool to see pending transaction
5. Mine a block as Charlie
6. View blockchain to see mined block
7. View UTXO set to see updated balances

## Educational Value

This simulator helps understand:
- How Bitcoin tracks ownership through UTXOs
- The lifecycle of a transaction (creation → mempool → block)
- How fees incentivize miners
- Blockchain structure and growth
- Double-spending prevention mechanisms
- Change calculation and UTXO consolidation

## Limitations

This is a simplified educational model and differs from real Bitcoin in several ways:
- No cryptographic signatures or verification
- Simplified fee calculation
- No difficulty adjustment or proof-of-work
- No peer-to-peer network simulation
- Fixed block size (5 transactions)
- Simplified validation rules

## Contributing

This is an educational project for CS216. Contributions should maintain the educational focus and code clarity.

## License

This project is created for educational purposes as part of CS216 coursework.

## Authors

Created as part of the CS216 - nChain AI UTXO Simulator assignment.

## Acknowledgments

- Bitcoin whitepaper by Satoshi Nakamoto
- CS216 course materials
- nChain AI for assignment design
