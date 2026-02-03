#include <set>
#include <iostream>
#include "transaction.h"
#include "utxomanager.h"
#include "validator.h"
using namespace std;

pair<bool,string> validateTransaction(Transaction& tx, UTXOManager& utxo_manager,double &InputSum,double &OutputSum,set<pair<string,int>>& spent_utxos) {
    vector<TransactionInput> Inputs = tx.getTransactionInputs();
    vector<TransactionOutput> Outputs = tx.getTransactionOutputs();

    set<pair<string,int>> seen_inputs;

    for(auto Input:Inputs){
        string tx_id=Input.GetTransactionId();
        int index=Input.GetTransactionIndex();
        string owner=Input.GetTransactionOwner();

        if(!utxo_manager.exists(tx_id,index)) {
            return {false,"UTXO is invalid"};
        }

        if(owner!=utxo_manager.GetUTXODetails(tx_id,index).second) {

            return {false,"UTXO does not belong to this owner"};
        }

        if(seen_inputs.find({tx_id,index})==seen_inputs.end()) {
            seen_inputs.insert({tx_id,index});
        }
        else {
            cout<<"User is trying to double spend ("<<tx_id<<","<<index<<") "<<endl;

            return {false,"Double Spending."};
        }
        
        if(spent_utxos.find({tx_id,index})!=spent_utxos.end()) {
            cout<<"UTXO with ("<<tx_id<<","<<index<<") has already been spent"<<endl;
            return {false,"UTXO is already spent"};
        }

        InputSum+=utxo_manager.GetUTXODetails(tx_id,index).first;
    }

    for(auto output:Outputs) {
        double currOutput=output.GetTransactionAmount();
        if(currOutput<0) {
            return {false,"Output amount cannot negative"};
        }
        OutputSum+=currOutput;
    }

    if(OutputSum>InputSum) {
        return {false,"Insufficient balance or input amount"};
    }

    return {true,"Valid"};
}