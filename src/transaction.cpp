#include <string>
#include <vector>
using namespace std;
 
class TransactionInput {
    private:

        string tx_id;
        int index;
        string owner;

    public:

        TransactionInput(string tx_id1,int index1,string owner1) {
            tx_id=tx_id1;
            index=index1;
            owner=owner1;
        } 

        string GetTransactionId() {
            return tx_id;
        }

        int GetTransactionIndex() {
            return index;
        }

        string GetTransactionOwner() {
            return owner;
        }
};

class TransactionOutput {
    private:

        double amount;
        string owner;

    public:

        TransactionOutput(double amount1,string owner1) {
            amount=amount1;
            owner=owner1;
        }

        double GetTransactionAmount() {
            return amount;
        }

        string GetTransactionOwner() {
            return owner;
        }



};

class Transaction {
    private:

        string tx_id;
        vector<TransactionInput> inputs;
        vector<TransactionOutput> outputs;

    public:

        Transaction(string tx_id1,vector<TransactionInput> Inputs1,vector<TransactionOutput> Outputs1) {
            tx_id=tx_id1;
            inputs=Inputs1;
            outputs=Outputs1;
        }

        vector<TransactionInput> getTransactionInputs() {
            return inputs;
        }

        vector<TransactionOutput> getTransactionOutputs() {
            return outputs;
        }

        string getTransactionID() {
            return tx_id;
        }

        bool operator<(const Transaction &t1) const
        {
            return (tx_id<t1.tx_id);
        }
};  