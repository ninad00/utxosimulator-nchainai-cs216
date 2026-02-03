#include<bits/stdc++.h>
#include"mempool.h"
using namespace std;

struct Block {
    int block_height;
    string miner;
    vector<Transaction> transactions;
    double total_fees;
    double block_reward;
    string timestamp;

    void display() {
        cout << "\n================ BLOCK " << block_height << " ================" << endl;
        cout << "Miner: " << miner << endl;
        cout << "Timestamp: " << timestamp << endl;
        cout << "Block Reward: " << fixed << setprecision(3) << block_reward << " BTC" << endl;
        cout << "Total Fees: " << total_fees << " BTC" << endl;
        cout << "Total Transactions: " << transactions.size() << endl;
        cout << "---------------- Transactions ----------------" << endl;
        for (const auto& tx : transactions) {
            cout << "  TX ID: " << tx.tx_id << " | Fee: " << tx.fee << " BTC" << endl;
            cout << "    Inputs: " << tx.inputs.size() << " | Outputs: " << tx.outputs.size() << endl;
            for (const auto& out : tx.outputs) {
                cout << "      -> " << out.address << ": " << out.amount << " BTC" << endl;
            }
        }
        cout << "===============================================" << endl;
    }
};

Block mine_block(string miner_address, Mempool& mempool, UTXOManager& utxo_manager, int block_height, double block_reward, int num_txs = 5) {
    vector<Transaction> to_mine = mempool.get_top_transactions(num_txs);
    if(to_mine.empty()) 
    {
        cout << "No transactions to mine." << endl;
        return { -1, "", {}, 0, 0, "" };
    }

    cout << "Mining block..." << endl;
    cout << "Selected " << to_mine.size() << " transactions from mempool." << endl;

    double total_fees = 0;
    for(const auto& tx : to_mine) 
    {
        // remove spent input utxos from UTXO set
        for(const auto& input : tx.inputs) 
        {
            utxo_manager.remove_utxo(input.prev_tx_id, input.index);
        }
        // add output utxos to UTXO set
        for(size_t i = 0; i < tx.outputs.size(); ++i) 
        {
            utxo_manager.add_utxo(tx.tx_id, i, tx.outputs[i].amount, tx.outputs[i].address);
        }
        total_fees += tx.fee;
        // remove from mempool
        mempool.remove_transaction(tx.tx_id);
    }

    // block reward + total fees
    double total_miner_reward = block_reward + total_fees;
    string coinbase_tx_id = "coinbase_block_" + to_string(block_height);
    utxo_manager.add_utxo(coinbase_tx_id, 0, total_miner_reward, miner_address);

    cout << "Block Reward: " << fixed << setprecision(3) << block_reward << " BTC" << endl;
    cout << "Total fees: " << fixed << setprecision(3) << total_fees << " BTC" << endl;
    cout << "Miner " << miner_address << " receives " << total_miner_reward << " BTC" << endl;
    cout << "Block mined successfully!" << endl;

    time_t now = time(0); // time stamp for header
    char* dt = ctime(&now);
    string timestamp(dt);
    if (!timestamp.empty() && timestamp.back() == '\n') timestamp.pop_back();

    return { block_height, miner_address, to_mine, total_fees, block_reward, timestamp };
}
