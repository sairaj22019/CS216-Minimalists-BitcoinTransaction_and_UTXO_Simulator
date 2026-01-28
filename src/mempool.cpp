#include <set>
#include <iostream>
#include <algorithm>
#include "transaction.h"
#include "utxomanager.h"
#include "validator.h"
using namespace std;

class Mempool {
    private:

        int capacity;
        set<pair<string,int>> spent_utxos; // set<tx_id,index>
        vector<pair<int,Transaction>> transactions;

    public:

        Mempool() {
            capacity = 100;
        }

        Mempool(int max_size) {
            capacity = max_size;
        }

        pair<bool,string> add_transaction(Transaction& tx, UTXOManager& utxo_manager) {
            vector<TransactionInput> Inputs = tx.getTransactionInputs();
            vector<TransactionOutput> Outputs = tx.getTransactionOutputs();

            double InputSum = 0;
            double OutputSum = 0;
            
            pair<bool,string> valid = validateTransaction(tx,utxo_manager,InputSum,OutputSum,spent_utxos);

            if(!valid.first) {
                return valid;
            }

            for(auto Input:Inputs) {
                string tx_id=Input.GetTransactionId();
                int index=Input.GetTransactionIndex();
                spent_utxos.insert({tx_id,index});
            }

            transactions.push_back({InputSum-OutputSum , tx});

            sort(transactions.rbegin(),transactions.rend());
            if(transactions.size() > capacity) {
                int rem = transactions.size() - capacity;
                while(rem > 0) {
                    transactions.pop_back();
                    rem--;
                }
            }

            return {true,"Transaction Successful"};
        }

        vector<Transaction> get_top_transactions(int n) {
            sort(transactions.begin(),transactions.end());
            vector<Transaction> confirmedTransactions;
            int size=transactions.size();

            int selectedSize=min(n,size);

            while(selectedSize--) {
                confirmedTransactions.push_back(transactions.back().second);
                transactions.pop_back();
            }

            return confirmedTransactions;
        }

        void clear() {
            capacity = 0;
            transactions.clear();
            spent_utxos.clear();
        }

        int getCapacity() {
            return capacity;
        }

        set<pair<string,int>> getSpent_utxo() {
            return spent_utxos;
        }

        vector<pair<int,Transaction>> getTransactions() {
            return transactions;
        }
};

