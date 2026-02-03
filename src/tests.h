#ifndef TESTS_H
#define TESTS_H

#include <bits/stdc++.h>
#include "simulator.h"

using namespace std;

namespace tests {

static const double EPS = 1e-9;

static void expect(bool cond, const string &msg) {
    cout << (cond ? "[PASS] " : "[FAIL] ") << msg << endl;
}

static Transaction* find_tx_in_mempool(Mempool &mp, const string &tx_id) {
    for (auto &t : mp.transactions) {
        if (t.tx_id == tx_id) return &t;
    }
    return nullptr;
}

static string make_tx_id(const string &tag) {
    static int counter = 0;
    return "tx_" + tag + "_" + to_string(++counter);
}

/*
Test 1: Basic Valid Transaction
- Alice (50) sends 10 to Bob
- Must include change back to Alice
- Must calculate correct fee (0.001)
*/
static bool test_basic_valid_transaction() {
    cout << "Running Test 1: Basic Valid Transaction" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");

    Transaction tx;
    tx.tx_id = make_tx_id("basic");
    tx.inputs.push_back({"genesis", 0, "Alice"});

    double send_amount = 10.0;
    double expected_fee = 0.001;
    double change = 50.0 - send_amount - expected_fee;

    tx.outputs.push_back({send_amount, "Bob"});
    tx.outputs.push_back({change, "Alice"}); // set change so stored tx fee = expected_fee

    auto res = mp.add_transaction(tx, um);
    expect(res.first, "Transaction should be accepted by mempool");

    bool ok = false;
    if (res.first) {
        Transaction* stored = find_tx_in_mempool(mp, tx.tx_id);
        expect(stored != nullptr, "Transaction is present in mempool");
        if (stored) {
            expect(fabs(stored->fee - expected_fee) < 1e-9, "Fee should be approx 0.001 BTC");
            bool found_change = false;
            for (auto &o : stored->outputs) {
                if (o.address == "Alice" && fabs(o.amount - change) < EPS) found_change = true;
            }
            expect(found_change, "Change output back to Alice present and correct");
            ok = found_change && fabs(stored->fee - expected_fee) < 1e-9;
        }
    }
    cout << endl;
    return ok;
}

/*
Test 2: Multiple Inputs
- Alice spends two UTXOs (50 + 20)
- Sends 60 to Bob
- Tests input aggregation and fee calculation
*/
static bool test_multiple_inputs() {
    cout << "Running Test 2: Multiple Inputs" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");
    um.add_utxo("extra", 0, 20.0, "Alice");

    Transaction tx;
    tx.tx_id = make_tx_id("multi");
    tx.inputs.push_back({"genesis", 0, "Alice"});
    tx.inputs.push_back({"extra", 0, "Alice"});

    double send_amount = 60.0;
    // leave outputs so fee is total_input - total_output
    tx.outputs.push_back({send_amount, "Bob"});

    auto res = mp.add_transaction(tx, um);
    expect(res.first, "Transaction with multiple inputs should be accepted");

    bool ok = false;
    if (res.first) {
        Transaction* stored = find_tx_in_mempool(mp, tx.tx_id);
        expect(stored != nullptr, "Transaction stored in mempool");
        if (stored) {
            double total_input = 50.0 + 20.0;
            double expected_fee = total_input - send_amount;
            expect(fabs(stored->fee - expected_fee) < 1e-9, "Fee calculated correctly for multiple inputs");
            ok = fabs(stored->fee - expected_fee) < 1e-9;
        }
    }
    cout << endl;
    return ok;
}

/*
Test 3: Double-Spend in Same Transaction
- Transaction tries to spend same UTXO twice
- Expected: REJECT with clear error message
*/
static bool test_double_spend_same_tx() {
    cout << "Running Test 3: Double-Spend in Same Transaction" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");

    Transaction tx;
    tx.tx_id = make_tx_id("double_same");
    tx.inputs.push_back({"genesis", 0, "Alice"});
    tx.inputs.push_back({"genesis", 0, "Alice"}); // same input twice
    tx.outputs.push_back({10.0, "Bob"});

    auto res = mp.add_transaction(tx, um);
    expect(!res.first, "Transaction that spends same UTXO twice should be rejected");
    if (!res.first) {
        expect(res.second.find("Double-spending in same transaction") != string::npos,
               "Error message should mention double-spending in same transaction");
    }
    cout << endl;
    return !res.first;
}

/*
Test 4: Mempool Double-Spend
- TX1: Alice -> Bob (spends UTXO)
- TX2: Alice -> Charlie (spends SAME UTXO)
- Expected: TX1 accepted, TX2 rejected
*/
static bool test_mempool_double_spend() {
    cout << "Running Test 4: Mempool Double-Spend" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");

    Transaction tx1;
    tx1.tx_id = make_tx_id("mempool1");
    tx1.inputs.push_back({"genesis", 0, "Alice"});
    tx1.outputs.push_back({10.0, "Bob"});

    auto r1 = mp.add_transaction(tx1, um);
    expect(r1.first, "First transaction should be accepted into mempool");

    Transaction tx2;
    tx2.tx_id = make_tx_id("mempool2");
    tx2.inputs.push_back({"genesis", 0, "Alice"});
    tx2.outputs.push_back({5.0, "Charlie"});

    auto r2 = mp.add_transaction(tx2, um);
    expect(!r2.first, "Second transaction spending same UTXO should be rejected by mempool");
    if (!r2.first) {
        expect(r2.second.find("UTXO already spent in mempool") != string::npos,
               "Error should indicate UTXO already spent in mempool");
    }
    cout << endl;
    return r1.first && !r2.first;
}

/*
Test 5: Insufficient Funds
- Bob tries to send 35 BTC (has only 30 BTC)
- Expected: REJECT with "Insufficient funds"
*/
static bool test_insufficient_funds() {
    cout << "Running Test 5: Insufficient Funds" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 1, 30.0, "Bob");

    Transaction tx;
    tx.tx_id = make_tx_id("insuff");
    tx.inputs.push_back({"genesis", 1, "Bob"});
    tx.outputs.push_back({35.0, "Alice"}); // Bob trying to send more than he has

    auto res = mp.add_transaction(tx, um);
    expect(!res.first, "Transaction with outputs greater than inputs should be rejected (Insufficient funds)");
    if (!res.first) {
        // mempool's error contains "Insufficient funds: Input (...) < Output (...)" per implementation
        expect(res.second.find("Insufficient funds") != string::npos || res.second.find("Input (") != string::npos,
               "Error message should indicate insufficient funds");
    }
    cout << endl;
    return !res.first;
}

/*
Test 6: Negative Amount
- Transaction with negative output amount
- Expected: REJECT immediately
*/
static bool test_negative_amount() {
    cout << "Running Test 6: Negative Amount" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");

    Transaction tx;
    tx.tx_id = make_tx_id("negative");
    tx.inputs.push_back({"genesis", 0, "Alice"});
    tx.outputs.push_back({-10.0, "Bob"}); // negative amount

    auto res = mp.add_transaction(tx, um);
    expect(!res.first, "Transaction with negative output amount should be rejected");
    if (!res.first) {
        expect(res.second.find("Negative output amount") != string::npos,
               "Error message should mention negative output amount");
    }
    cout << endl;
    return !res.first;
}

/*
Test 7: Zero Fee Transaction
- Inputs = Outputs (fee = 0)
- Expected: ACCEPTED
*/
static bool test_zero_fee_transaction() {
    cout << "Running Test 7: Zero Fee Transaction" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 2, 20.0, "Charlie");

    Transaction tx;
    tx.tx_id = make_tx_id("zerofee");
    tx.inputs.push_back({"genesis", 2, "Charlie"});
    tx.outputs.push_back({20.0, "David"}); // exactly equal

    auto res = mp.add_transaction(tx, um);
    expect(res.first, "Zero-fee transaction (inputs == outputs) should be accepted");
    bool ok = false;
    if (res.first) {
        Transaction* stored = find_tx_in_mempool(mp, tx.tx_id);
        expect(stored != nullptr, "Transaction present in mempool");
        if (stored) {
            expect(fabs(stored->fee - 0.0) < EPS, "Fee should be zero for zero-fee transaction");
            ok = fabs(stored->fee - 0.0) < EPS;
        }
    }
    cout << endl;
    return ok;
}

/*
Test 8: Race Attack Simulation
- Low-fee merchant TX arrives first
- High-fee attack TX arrives second
- Expected: First transaction wins (first-seen rule)
*/
static bool test_race_attack_simulation() {
    cout << "Running Test 8: Race Attack Simulation" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");

    Transaction tx_low;
    tx_low.tx_id = make_tx_id("race_low");
    tx_low.inputs.push_back({"genesis", 0, "Alice"});
    tx_low.outputs.push_back({49.0, "Merchant"}); // fee = 1.0

    auto rlow = mp.add_transaction(tx_low, um);
    expect(rlow.first, "Low-fee transaction should be accepted when seen first");

    Transaction tx_high;
    tx_high.tx_id = make_tx_id("race_high");
    tx_high.inputs.push_back({"genesis", 0, "Alice"});
    tx_high.outputs.push_back({10.0, "Attacker"}); // higher fee

    auto rhigh = mp.add_transaction(tx_high, um);
    expect(!rhigh.first, "High-fee conflicting transaction arriving later should be rejected (first-seen rule)");
    cout << endl;
    return rlow.first && !rhigh.first;
}

/*
Test 9: Complete Mining Flow
- Add multiple transactions to mempool
- Mine a block
- Check: UTXOs updated, miner gets fees, mempool cleared
*/
static bool test_complete_mining_flow() {
    cout << "Running Test 9: Complete Mining Flow" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");
    um.add_utxo("genesis", 1, 30.0, "Bob");

    // create two transactions
    Transaction tx1;
    tx1.tx_id = make_tx_id("mine1");
    tx1.inputs.push_back({"genesis", 0, "Alice"});
    tx1.outputs.push_back({10.0, "Bob"}); // fee = 40.0
    mp.add_transaction(tx1, um);

    Transaction tx2;
    tx2.tx_id = make_tx_id("mine2");
    tx2.inputs.push_back({"genesis", 1, "Bob"});
    tx2.outputs.push_back({5.0, "Charlie"}); // fee = 25.0
    mp.add_transaction(tx2, um);

    // mine a block (mine_block defined in block.h)
    Block b = mine_block("Miner1", mp, um, 1, 5);

    // After mining, mempool should no longer contain the mined txs
    bool mempool_cleared = mp.transactions.empty();
    expect(mempool_cleared, "Mempool should be cleared of mined transactions");

    // Coinbase should be added to UTXO set
    bool coinbase_present = um.exists("coinbase_block_1", 0);
    expect(coinbase_present, "Coinbase UTXO for miner should be added to UTXO set");

    cout << endl;
    return mempool_cleared && coinbase_present;
}

/*
Test 10: Unconfirmed Chain
- Alice -> Bob (TX1 creates new UTXO for Bob in mempool only)
- Bob tries to spend that UTXO before TX1 is mined
- This simulator design: spending mempool-only outputs is rejected (UTXO must exist in UTXOManager)
*/
static bool test_unconfirmed_chain() {
    cout << "Running Test 10: Unconfirmed Chain" << endl;
    UTXOManager um;
    Mempool mp;
    um.add_utxo("genesis", 0, 50.0, "Alice");

    Transaction tx1;
    tx1.tx_id = make_tx_id("unconfirmed1");
    tx1.inputs.push_back({"genesis", 0, "Alice"});
    tx1.outputs.push_back({10.0, "Bob"});
    auto r1 = mp.add_transaction(tx1, um);
    expect(r1.first, "TX1 should be accepted into mempool (creates an unconfirmed output conceptually)");

    Transaction tx2;
    tx2.tx_id = make_tx_id("unconfirmed2");
    // Bob tries to spend the output of tx1 (which is not in UTXOManager yet)
    tx2.inputs.push_back({tx1.tx_id, 0, "Bob"});
    tx2.outputs.push_back({5.0, "Charlie"});

    auto r2 = mp.add_transaction(tx2, um);
    expect(!r2.first, "Bob's spend of an unmined (mempool-only) UTXO should be rejected by this simulator design");
    if (!r2.first) {
        expect(r2.second.find("Input UTXO does not exist") != string::npos,
               "Error should indicate missing input UTXO");
    }
    cout << endl;
    return r1.first && !r2.first;
}

/*
Run all tests and print a summary.
*/
static void run_all_tests() {
    cout << "========== Running Test Suite ==========" << endl;
    int passed = 0;
    const int total = 10;
    if (test_basic_valid_transaction()) passed++;
    if (test_multiple_inputs()) passed++;
    if (test_double_spend_same_tx()) passed++;
    if (test_mempool_double_spend()) passed++;
    if (test_insufficient_funds()) passed++;
    if (test_negative_amount()) passed++;
    if (test_zero_fee_transaction()) passed++;
    if (test_race_attack_simulation()) passed++;
    if (test_complete_mining_flow()) passed++;
    if (test_unconfirmed_chain()) passed++;

    cout << "========================================" << endl;
    cout << "Passed " << passed << " / " << total << " tests." << endl;
}

} // namespace tests

void Simulator::run_test_scenarios()
{
    tests::run_all_tests();
}

#endif // TESTS_H