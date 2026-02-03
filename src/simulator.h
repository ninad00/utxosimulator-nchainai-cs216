#ifndef SIMULATOR_H
#define SIMULATOR_H

#include<bits/stdc++.h>
#include"block.h"
#include "tests.h"

using namespace std;

const double epsilon = 1e-9;
static int counter = 0;

class Simulator 
{
    UTXOManager utxo_manager;
    Mempool mempool;
    vector<Block> blockchain;
    double gas_fee;
    double block_reward;
    int k;
public:
    Simulator(double gas_fee = 0.0, double block_reward = 12, int mempool_maxsize = 5, int k = 3) 
    {
        this->gas_fee = gas_fee;
        mempool = Mempool(mempool_maxsize, gas_fee);
        this->block_reward = block_reward;
        if(k<=0)
        {
            k=3;
        }
        this->k = k;
        utxo_manager.add_utxo("genesis", 0, 50.0, "Alice");
        utxo_manager.add_utxo("genesis", 1, 30.0, "Bob");
        utxo_manager.add_utxo("genesis", 2, 20.0, "Charlie");
        utxo_manager.add_utxo("genesis", 3, 10.0, "David");
        utxo_manager.add_utxo("genesis", 4, 5.0, "Eve");
    }

    void run_test_scenarios(int, double);

    void create_transaction_ui() 
    {
        string sender, recipient;
        double amount;
        cout << "Enter sender: ";
        cin >> sender;
        
        double balance = utxo_manager.get_balance(sender);
        cout << "Available balance: " << balance << " BTC" << endl;
        
        if(balance == 0) //sender is broke
        {
            cout << "Error: Sender has no balance." << endl;
            return;
        }

        cout << "Enter recipient: ";
        cin >> recipient;
        cout << "Enter amount: ";
        cin >> amount;

        if(amount > balance) //cant send more than u have
        {
            cout << "Error: Insufficient funds." << endl;
            return;
        }

        vector<UTXO> sender_utxos = utxo_manager.get_utxos_for_owner(sender);
        Transaction tx;
        tx.tx_id = "tx_" + sender + "_" + recipient + "_" + to_string(rand() % 1000);
        
        double total_input = 0;


        for(const auto& utxo : sender_utxos) 
        {
            //double spend in same transaction
            if(mempool.spent_utxos.count({utxo.tx_id, utxo.index}))
                 continue;
            
            tx.inputs.push_back({utxo.tx_id, utxo.index, utxo.owner});
            total_input += utxo.amount;
            
            //dont have to use all utxos
            if (total_input >= amount*(1 + gas_fee)) 
            break; 
        }
        
        //too less funds that are not in mempool
        if(total_input + epsilon < amount) 
        {
            cout << "Error: All available UTXOs are already pending in mempool." << endl;
            return;
        }
        else if(total_input + epsilon < amount*(1+gas_fee))
        {
            //cant pay fee
            cout << "Error: Insufficient funds for fee." << endl;
            return;
        }

        tx.outputs.push_back({amount, recipient});

        double change = total_input - amount*(1+gas_fee);
        if(abs(change) < epsilon)
            change = 0;
            
        if(change > 0) 
        {
            tx.outputs.push_back({change, sender});
        } 

        cout << "Creating transaction..." << endl;
        auto result = mempool.add_transaction(tx, utxo_manager);
        cout << result.second << endl;
        if (result.first) {
            cout << "Transaction ID: " << tx.tx_id << endl;
            cout << "Transaction added to mempool." << endl;
            cout << "Mempool now has " << mempool.transactions.size() << " transactions." << endl;
        }
    }

    void display_blockchain() {
        if (blockchain.empty()) {
            cout << "Blockchain is empty. No blocks mined yet." << endl;
            return;
        }
        for (int i = 0; i < blockchain.size(); ++i) {
            int box_width = blockchain[i].display();
            if (i < blockchain.size() - 1) {
                int dashes = box_width - 2; 
                int center_pos = box_width / 2 - 3;
                cout << endl; 
                cout << "  " << string(center_pos - 1, ' ') << "|" << endl;
                cout << "  " << string(center_pos - 1, ' ') << "v" << endl;
            }
            
        }
    }

    void run() {
        cout << "=== Bitcoin Transaction Simulator ===" << endl;
        cout << "Initial UTXOs (Genesis Block):" << endl;
        utxo_manager.display();

        while (true) 
        {
            cout << "\nMain Menu:" << endl;
            cout << "1. Create new transaction" << endl;
            cout << "2. View UTXO set" << endl;
            cout << "3. View mempool" << endl;
            cout << "4. Mine block" << endl;
            cout << "5. View blockchain" << endl;
            cout << "6. Run test scenarios" << endl;
            cout << "7. Exit" << endl;
            cout << "Enter choice: ";
            
            int choice;
            if(!(cin >> choice)) 
            {
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }

            if(choice == 1) 
                create_transaction_ui();
            else if (choice == 2) 
                utxo_manager.display();
            else if (choice == 3) 
                mempool.display();
            else if (choice == 4) 
            {
                string miner;
                cout << "Enter miner name: ";
                cin >> miner;
                counter++;
                Block new_block = mine_block(miner, mempool, utxo_manager, blockchain.size() + 1, block_reward);
                if(counter%k==0)
                block_reward/=2;
                if (new_block.block_height != -1) {
                    blockchain.push_back(new_block);
                }
            } 
            else if (choice == 5)
                display_blockchain();
            else if (choice == 6) 
                run_test_scenarios(k, gas_fee);
            else if (choice == 7) 
                break;
            else 
                cout << "Invalid choice." << endl;
        }
    }
};

#endif
