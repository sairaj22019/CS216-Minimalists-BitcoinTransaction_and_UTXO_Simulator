#include <set>
#include <iostream>
#include "transaction.h"
#include "utxomanager.h"
using namespace std;

pair<bool,string> validateTransaction(Transaction& tx, UTXOManager& utxo_manager,double &InputSum,double &OutputSum,set<pair<string,int>>& spent_utxos) {
            vector<TransactionInput> Inputs = tx.getTransactionInputs();
            vector<TransactionOutput> Outputs = tx.getTransactionOutputs();

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
                double currOutput=output.GetTransactionAmount();
                if(currOutput<0) {
                    return {false,"Output amount cannot negative"};
                }
                OutputSum+=currOutput;
            }

            if(OutputSum>InputSum) {
                return {false,"Transaction Invalid"};
            }

            return {true,"Valid"};
        }