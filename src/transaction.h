#pragma once
#include <vector>
#include <string>
using namespace std;
 
class TransactionInput {
    private:

        string tx_id;
        int index;
        string owner;

    public:

        TransactionInput(string tx_id1,int index1,string owner1);
        string GetTransactionId();
        int GetTransactionIndex();
        string GetTransactionOwner();

};

class TransactionOutput {
    private:

        double amount;
        string owner;

    public:

        TransactionOutput(double amount1,string owner1);
        double GetTransactionAmount();
        string GetTransactionOwner();

};

class Transaction {
    private:

        string tx_id;
        vector<TransactionInput> inputs;
        vector<TransactionOutput> outputs;

    public:

        Transaction(string tx_id1,vector<TransactionInput> Inputs1,vector<TransactionOutput> Outputs1);
        
        vector<TransactionInput> getTransactionInputs();

        vector<TransactionOutput> getTransactionOutputs();

        string getTransactionID();
        
        bool operator<(const Transaction &t1) const;
};