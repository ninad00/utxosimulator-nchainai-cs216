#include<bits/stdc++.h>
#include"transaction.h"
using namespace std;

class Mempool 
{
public:
    vector<Transaction> transactions;
    set<pair<string, int>> spent_utxos;
    int max_size;
    Mempool(int max_size = 50)
    {
        this->max_size = max_size; //default maxsize
    }

    //add transaction to mempool
    pair<bool, string> add_transaction(Transaction tx, UTXOManager& utxo_manager) 
    {

        // Validation 1: Inputs exist in UTXO set
        double total_input = 0;
        set<pair<string, int>> tx_inputs;
        for (const auto& input : tx.inputs) 
        {
            // does utxo exists?
            if(!utxo_manager.exists(input.prev_tx_id, input.index)) 
            {
                return {false, "Input UTXO does not exist: " + input.prev_tx_id + ":" + to_string(input.index)};
            }

            // am i using same utxo in same transaction?
            if(tx_inputs.count({input.prev_tx_id, input.index})) 
            {
                return {false, "Double-spending in same transaction: " + input.prev_tx_id + ":" + to_string(input.index)};
            }

            tx_inputs.insert({input.prev_tx_id, input.index});
            
            // am i using a spent utxo?
            if(spent_utxos.count({input.prev_tx_id, input.index})) 
            {
                return {false, "UTXO already spent in mempool: " + input.prev_tx_id + ":" + to_string(input.index)};
            }

            total_input += utxo_manager.utxo_set[{input.prev_tx_id, input.index}].amount;
        }

        double total_output = 0;
        for (const auto& output : tx.outputs) {
            // am i sending negative money?
            if (output.amount < 0) {
                return {false, "Negative output amount"};
            }
            total_output += output.amount;
        }
        // am i spending more than what i have?
        if (total_input < total_output) {
            return {false, "Insufficient funds: Input (" + to_string(total_input) + ") < Output (" + to_string(total_output) + ")"};
        }


        //need to improve
        tx.fee = total_input - total_output;

        transactions.push_back(tx);

        for (const auto& input : tx.inputs) 
        {
            spent_utxos.insert({input.prev_tx_id, input.index});
        }
        if(transactions.size() > max_size)
        {
            transactions = get_top_transactions(max_size);
        }

        return {true, "Transaction valid! Fee: " + to_string(tx.fee)};
    }

    void remove_transaction(string tx_id) 
    {
        for (auto it = transactions.begin(); it != transactions.end(); ++it) 
        {
            if (it->tx_id == tx_id) 
            {
                for (const auto& input : it->inputs) 
                {
                    spent_utxos.erase({input.prev_tx_id, input.index});
                }
                transactions.erase(it);
                return;
            }
        }
    }

    vector<Transaction> get_top_transactions(int n) 
    {
        vector<Transaction> sorted_txs = transactions;
        sort(sorted_txs.begin(), sorted_txs.end(), [](const Transaction& a, const Transaction& b) {
            return a.fee > b.fee; //high fee priority
        });
        if(sorted_txs.size() > n) 
        {
            sorted_txs.resize(n);
        }
        return sorted_txs;
    }

    void clear() //clear mempool
    {
        transactions.clear();
        spent_utxos.clear();
    }

    void display() 
    {
        cout << "\n--- Current Mempool ---" << endl;
        if(transactions.empty()) 
        {
            cout << "Mempool is empty." << endl;
            return;
        }
        cout << left << setw(25) << "TX ID" << setw(10) << "Inputs" << setw(10) << "Outputs" << setw(10) << "Fee" << endl;
        for(const auto& tx : transactions) 
        {
            cout << left << setw(25) << tx.tx_id << setw(10) << tx.inputs.size() << setw(10) << tx.outputs.size() << setw(10) << fixed << setprecision(3) << tx.fee << endl;
        }
    }
};
