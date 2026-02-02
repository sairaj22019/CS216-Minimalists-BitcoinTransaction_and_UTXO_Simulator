#include "transaction.h"
using namespace std;


TransactionInput::TransactionInput(string tx_id1, int index1, string owner1) {
    tx_id = tx_id1;
    index = index1;
    owner = owner1;
}

string TransactionInput::GetTransactionId() {
    return tx_id;
}

int TransactionInput::GetTransactionIndex() {
    return index;
}

string TransactionInput::GetTransactionOwner() {
    return owner;
}

TransactionOutput::TransactionOutput(double amount1, string owner1) {
    amount = amount1;
    owner = owner1;
}

double TransactionOutput::GetTransactionAmount() {
    return amount;
}

string TransactionOutput::GetTransactionOwner() {
    return owner;
}

Transaction::Transaction(string tx_id1,
    vector<TransactionInput> Inputs1,
    vector<TransactionOutput> Outputs1) {

    tx_id = tx_id1;
    inputs = Inputs1;
    outputs = Outputs1;
}

vector<TransactionInput> Transaction::getTransactionInputs() {
    return inputs;
}

vector<TransactionOutput> Transaction::getTransactionOutputs() {
    return outputs;
}

string Transaction::getTransactionID() {
    return tx_id;
}

bool Transaction::operator<(const Transaction &t1) const {
    return tx_id < t1.tx_id;
}

