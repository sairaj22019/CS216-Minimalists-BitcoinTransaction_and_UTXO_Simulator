#include <set>
#include <iostream>
#include <queue>
#include <algorithm>
#include "transaction.h"
#include "utxomanager.h"
using namespace std;

class Mempool {
    private:

        int capacity;
        set<pair<string,int>> spent_utxos; // set<tx_id,index>
        vector<Transaction> transactions;

    public:

        Mempool() {
            capacity = 50;
        }

        Mempool(int max_size) {
            capacity = max_size;
        }

        pair<bool,string> add_transaction(Transaction& tx, UTXOManager& utxo_manager) {
            
        }

        void remove_transaction(string tx_id) {
            for(auto tx = transactions.begin();tx!=transactions.end();tx++) {
                string compare_tx_id = tx->getTransactionID();
                if(tx_id == compare_tx_id) {
                    transactions.erase(tx);
                }
            }
        }

        vector<Transaction> get_top_transactions(int n) {
            sort(transactions.begin(),transactions.end()); //  Yet to do operator overloading.
        }

        void clear() {
            capacity = 0;
            transactions.clear();
            spent_utxos.clear();
        }

};