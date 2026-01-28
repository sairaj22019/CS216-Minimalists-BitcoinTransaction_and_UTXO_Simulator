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
            for(auto Input:Inputs){
                string tx_id=Input.GetTransactionId();
                int index=Input.GetTransactionIndex();
                string owner=Input.GetTransactionOwner();

                if(owner!=utxo_manager.GetUTXODetails(tx_id,index).second) {
                    return {false,"UTXO does not belong to this owner"};
                }

                if(!utxo_manager.exists(tx_id,index)){
                    return {false,"UTXO is invalid"};
                }

                if(spent_utxos.find({tx_id,index})!=spent_utxos.end()){
                    return {false,"UTXO is already spent"};
                }

                InputSum+=utxo_manager.GetUTXODetails(tx_id,index).first;
            }

            for(auto output:Outputs) {
                OutputSum+=output.GetTransactionAmount();
            }

            if(OutputSum>InputSum) {
                return {false,"Transaction Invalid"};
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

};

