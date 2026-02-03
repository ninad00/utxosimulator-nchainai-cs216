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

    // Prints the block as an ASCII box and returns the total printed box width
    int display() {
        vector<string> lines;
        // Header line
        lines.push_back("Block #" + to_string(block_height) + " | Miner: " + miner);
        // Time
        lines.push_back(string("Time: ") + timestamp);
        // Reward and fees line (formatted)
        {
            ostringstream oss;
            oss << fixed << setprecision(3) << "Reward: " << block_reward << " | Fees: " << total_fees << " BTC";
            lines.push_back(oss.str());
        }
        // TX count
        lines.push_back(string("TXs: ") + to_string(transactions.size()));
        // Transactions
        for (const auto& tx : transactions) {
            ostringstream oss;
            oss << "  +-- " << tx.tx_id.substr(0, 8) << "... [" << tx.inputs.size() << "->" << tx.outputs.size() << "] Fee: " << fixed << setprecision(3) << tx.fee;
            lines.push_back(oss.str());
        }

        // Determine max line length
        size_t max_len = 0;
        for (const auto& s : lines) max_len = max(max_len, s.size());

        // inner width includes a single space padding on each side
        size_t inner_width = max_len + 2; // one space left and right
        size_t total_width = inner_width + 2; // add the '+' borders

        string border = "+" + string(inner_width, '-') + "+";
        cout << "\n" << border << endl;
        for (const auto& s : lines) {
            cout << "| " << s << string(max_len - s.size(), ' ') << " |" << endl;
        }
        cout << border << endl;

        return static_cast<int>(total_width);
    }
};

Block mine_block(string miner_address, Mempool& mempool, UTXOManager& utxo_manager, int block_height, double block_reward, int num_txs = 4) {
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
