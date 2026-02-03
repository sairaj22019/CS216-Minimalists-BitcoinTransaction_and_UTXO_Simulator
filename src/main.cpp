#include <iostream>
#include <map>
#include "utxomanager.h"
#include "mempool.h"
#include "transaction.h"
#include "block.h"
#include <iomanip>
#include "../tests/tests.h"

using namespace std;

BlockChain blockChain;
UTXOManager utxo_manager;
Mempool mempool;
int tx_number = 0;


void generateGenesisBlock() {
    map<string,double> users = {
        {"Alice", 40.0},
        {"Bob", 50.0},
        {"Charlie", 70.0},
        {"Delta", 20.0},
        {"Eve", 10.0},
        {"Zoe", 80.0},
        {"Max", 40.0},
        {"El",100.0}
    };
    cout<<"--- Initial Genesis Block ---"<<endl;
    cout<<endl;
    string tx_id = "tx_genesis_000";
    int index = 0;
    for(auto user:users) {
        utxo_manager.add_utxo(tx_id,index,user.second,user.first);
        index++;
        cout<<"- "<<user.first<<" : "<<fixed<<setprecision(4)<<user.second<<" BTC"<<endl;
    }
}

void printChoices() {
    cout<<"Main Menu: \n"<<endl;
    cout<<"1. Create new transaction "<<endl;
    cout<<"2. View UTXO set "<<endl;
    cout<<"3. View mempool "<<endl;
    cout<<"4. Mine block "<<endl;
    cout<<"5. Run test scenarios "<<endl;
    cout<<"6. Print BlockChain "<<endl;
    cout<<"7. Exit "<<endl;
}

void createTransaction() {
    cout<<"Enter sender : ";
    string sender;
    cin>>sender;
    // tx_sender_recepient_txNo 
    double senderBalance = utxo_manager.get_balance(sender);
    cout<<"Available balance : "<<fixed<<setprecision(4)<<senderBalance<<endl;
    
    cout<<"Enter recipient : ";
    string recipient;
    cin>>recipient;

    cout<<"Enter amount : ";
    double amount;
    cin>>amount;

    cout<<endl;
    cout<<"Creating transaction..."<<endl;

    if(amount<=0){
        cout<<"Error: Please enter a positive amount"<<endl;
        return;
    }

    if(amount>senderBalance){
        cout<<"Error: Insufficient balance"<<endl;
        return;
    }

    vector<pair<pair<string,double>,pair<int,string>>> senderUTXOs = utxo_manager.get_utxos_for_owner(sender);
    auto spent_utxos = mempool.getSpent_utxo();

    double inputSum = 0;
    vector<TransactionInput> txInputs;
    for(auto utxo : senderUTXOs){
        double amount2 = utxo.first.second;
        string tx_id = utxo.second.second;
        int index = utxo.second.first;

        if(spent_utxos.find({tx_id,index}) == spent_utxos.end()){
            inputSum+=amount2;
            TransactionInput input(tx_id,index,sender);
            txInputs.push_back(input);

            if(inputSum>amount) break;
        }

    }

    if(inputSum<amount){
        cout<<"Error: double spending detected"<<endl;
        return;
    }
    double transactionFee = min(0.0001*amount,inputSum-amount);
    double outputSum = inputSum - transactionFee;
    TransactionOutput senderOutput(outputSum-amount,sender);
    TransactionOutput recipientOutput(amount,recipient);
    vector<TransactionOutput> txOutputs = {senderOutput,recipientOutput};
    

    string tx_id = "tx_";
    tx_id+=sender;
    tx_id+="_";
    tx_id+=recipient;
    tx_id+="_";
    tx_id+=to_string(tx_number);
    tx_number++;

    Transaction transaction(tx_id,txInputs,txOutputs);
    cout<<"Transaction valid!"<<endl;
    cout<<"Transaction Fee = "<<transactionFee<<endl;

    mempool.add_transaction(transaction,utxo_manager);
    cout<<"Transaction added to mempool"<<endl;

    cout<<"Mempool now has "<<mempool.getMempoolSize()<<" transactions"<<endl;
}

void viewUTXOSet() {
    cout<<endl;
    auto utxo_set = utxo_manager.get_utxo_set();
    cout<<"Owner        "<<"Amount      "<<"(tx_id,index)"<<endl<<endl;
    for(auto utxo : utxo_set){
        string tx_id = utxo.first.first;
        int index = utxo.first.second;
        double amount = utxo.second.first;
        string owner = utxo.second.second;

        cout<<owner<<"      "<<amount<<"        "<<"("<<tx_id<<","<<index<<")"<<endl;
    }
}

void viewMempool() {
    cout<<endl;
    vector<pair<double,Transaction>> mempoolTransaction = mempool.getTransactions();

    if(mempoolTransaction.size()==0){
        cout<<"Mempool is empty"<<endl;
        return;
    }

    cout<<"Transaction ids in mempool "<<endl;
    for(auto transaction : mempoolTransaction){
        cout<<transaction.second.getTransactionID()<<endl;
    }
}

void mineBlock() {
    cout<<"Enter miner name : ";
    string miner;
    cin>>miner;
    cout<<endl;
    cout<<"Mining block..."<<endl;

    if(mempool.getMempoolSize()==0){
        cout<<"Error: No transactions available in mempool to mine"<<endl;
        return;
    }

    int prev_size = mempool.getMempoolSize();
    cout<<"Selected "<<min(5,prev_size)<<" transactions from mempool"<<endl;
    
    auto spent_utxos = mempool.getSpent_utxo();

    blockChain.mine_block(miner,mempool,utxo_manager,spent_utxos,5);

    cout<<"Removed "<<min(5,prev_size)<<" transactions from mempool"<<endl;
}

void runTestScenarios() {
    cout<<"Enter Test Scenario number : ";
    int testNumber;
    cin>>testNumber;
    TestRunner testRunner;
    testRunner.runTest(testNumber);
}

int main() {
    cout<<"========== BLOCKCHAIN TRANSACTION SIMULATOR =========="<<endl;
    cout<<"\n"<<endl;

    generateGenesisBlock();

    while(true) {
        cout<<"\n"<<endl;
        
        printChoices();
        cout<<"\n";
        cout<<"Enter Input Choice : ";

        char s;
        cin>>s;

        if(!(('0'<=s)&&(s<='9'))) {
            cout<<"Please enter a digit in range of the given choices."<<endl;
            continue;
        }

        int n = s-'0';
        switch(n) {
            case 1:
                createTransaction();
                break;
            case 2:
                viewUTXOSet();
                break;
            case 3:
                viewMempool();
                break;
            case 4:
                mineBlock();
                break;
            case 5:
                runTestScenarios();
                break;
            case 6:
                blockChain.printBlockChain();
                break;
            case 7:
                cout<<"Program Terminated. Press Enter to Exit."<<endl;
                cin.get();
                cin.get();
                return 0;
                break;
            default:
                cout<<"Input is invalid. Select in the above given options."<<endl;
                break;
        }

    }
}