#include <set>
#include <iostream>
#include <algorithm>
#include "transaction.h"
#include "utxomanager.h"
#include "validator.h"
#include "mempool.h"
using namespace std;


Mempool::Mempool() {
    capacity = 10;
}

Mempool::Mempool(int max_size) {
    capacity = max_size;
}

pair<bool,string> Mempool::add_transaction(Transaction& tx, UTXOManager& utxo_manager) {
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

vector<Transaction> Mempool::get_top_and_pop_transactions(int n) {
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

void Mempool::clear() {
    capacity = 0;
    transactions.clear();
    spent_utxos.clear();
}

int Mempool::getCapacity() {
    return capacity;
}

set<pair<string,int>> Mempool::getSpent_utxo() {
    return spent_utxos;
}

void Mempool::updateSpent_utxo(set<pair<string,int>> &spent_utxo1)
{
    spent_utxos=spent_utxo1;
}

vector<pair<double,Transaction>> Mempool::getTransactions() {
    return transactions;
}

int Mempool::getMempoolSize() {
    return transactions.size();
}