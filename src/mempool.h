#pragma once
#include <set>
#include <iostream>
#include <algorithm>
#include "transaction.h"
#include "utxomanager.h"
using namespace std;

class Mempool {
    private:

        int capacity;
        set<pair<string,int>> spent_utxos; // set<tx_id,index>
        vector<pair<int,Transaction>> transactions;

    public:

        Mempool();
        Mempool(int max_size);
        pair<bool,string> add_transaction(Transaction& tx, UTXOManager& utxo_manager);
        vector<Transaction> get_top_transactions(int n);
        void clear();
        int getCapacity();
        set<pair<string,int>> getSpent_utxo();
        vector<pair<int,Transaction>> getTransactions();
};
