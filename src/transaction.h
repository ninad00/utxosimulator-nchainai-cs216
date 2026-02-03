#include<bits/stdc++.h>
#include"utxo_manager.h"
using namespace std;

//input format
struct Input 
{
    string prev_tx_id;
    int index;
    string owner; 
};

//output format
struct Output 
{
    double amount;
    string address;
};

struct Transaction 
{
    string tx_id;
    vector<Input> inputs;
    vector<Output> outputs;
    double fee;

    bool operator==(const Transaction& other) const {
        return tx_id == other.tx_id;
    }
};
