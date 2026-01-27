#include <set>
#include <iostream>
#include <queue>
#include "transaction.h"
#include "utxomanager.h"
using namespace std;

class Mempool {
    private:

        int capacity;
        set<pair<string,int>> spent_utxos; // set<tx_id,index>
        priority_queue<pair<double,Transaction>> transactions;

    public:

        Mempool() {
            capacity = 50;
        }

        Mempool(int max_size) {
            capacity = max_size;
        }

        pair<bool,string> add_transaction(Transaction tx, UTXOManager utxo_manager ) {
            
        }

        void remove_transaction(string tx_id) {

        }

        vector<Transaction> get_top_transactions(int n) {
            
        }

        void clear() {
            capacity = 0;
            transactions = priority_queue<pair<double,Transaction>>();
            spent_utxos.clear();
        }
};