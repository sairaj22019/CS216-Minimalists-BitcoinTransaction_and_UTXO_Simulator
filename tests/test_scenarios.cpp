#include <iostream>
#include <vector>
#include<iomanip>
#include "tests.h"
#include "../src/transaction.h"
#include "../src/utxomanager.h"
#include "../src/mempool.h"
#include "../src/block.h" 

using namespace std;

void setupTestEnv(UTXOManager& utxoMgr, Mempool& mempool) {
    mempool.clear();
    utxoMgr.add_utxo("tx_genesis_000", 0, 50.0, "Alice");
    utxoMgr.add_utxo("tx_genesis_000", 1, 30.0, "Bob");
    utxoMgr.add_utxo("tx_genesis_000", 2, 20.0, "Charlie");
    utxoMgr.add_utxo("tx_genesis_000", 3, 10.0, "David");
    utxoMgr.add_utxo("tx_genesis_000", 4, 5.0, "Eve");
}

void TestRunner::runTest(int testNumber) {
    cout << "\nRunning Test Scenario " << testNumber << "..." << endl;
    cout << "----------------------------------------" << endl;

    switch (testNumber) {
        case 1: test1_BasicValidTransaction(); break;
        case 2: test2_MultipleInputs(); break;
        case 3: test3_DoubleSpendSameTX(); break;
        case 4: test4_MempoolDoubleSpend(); break;
        case 5: test5_InsufficientFunds(); break;
        case 6: test6_NegativeAmount(); break;
        case 7: test7_ZeroFee(); break;
        case 8: test8_RaceAttack(); break;
        case 9: test9_MiningFlow(); break;
        case 10: test10_UnconfirmedChain(); break;
        default: cout << "Invalid Test Number!" << endl; break;
    }
    cout << "----------------------------------------\n" << endl;
}

// --- IMPLEMENTATION OF INDIVIDUAL TESTS ---

void TestRunner::test1_BasicValidTransaction() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    
    cout << "Test: Alice sends 10 BTC to Bob (Valid)" << endl;
    cout<<endl;
    
    vector<TransactionInput> inputs = { TransactionInput("tx_genesis_000", 0, "Alice") };
    vector<TransactionOutput> outputs = { 
        TransactionOutput(10.0, "Bob"), 
        TransactionOutput(39.999, "Alice") 
    };
    
    Transaction tx("tx1", inputs, outputs);

    double InputSum=0,OutputSum=0;
    for(auto &input:inputs){
        string trans_id=input.GetTransactionId();
        int index=input.GetTransactionIndex();

        InputSum+=utxoMgr.GetUTXODetails(trans_id,index).first;
    }

    for(auto &output:outputs){
        OutputSum+=output.GetTransactionAmount();
    }


    
    auto res = mempool.add_transaction(tx, utxoMgr);
    if(res.first){
        cout << "[PASS] Transaction Accepted." << endl;
        cout<<"Transaction Fee: "<<fixed<<setprecision(4)<<InputSum-OutputSum<<endl;
    }
    else cout << "[FAIL] Transaction Rejected: " << res.second << endl;
}

void TestRunner::test2_MultipleInputs() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    utxoMgr.add_utxo("genesis_tx_001", 0, 20.0, "Alice");

    cout << "Test: Alice spends two UTXOs (50 + 20) to send 60" << endl;
    cout<<endl;

    vector<TransactionInput> inputs = { 
        TransactionInput("tx_genesis_000", 0, "Alice"),
        TransactionInput("genesis_tx_001", 0, "Alice")
    };
    vector<TransactionOutput> outputs = { 
        TransactionOutput(60.0, "Bob"),
        TransactionOutput(9.9, "Alice") 
    };
    Transaction tx("tx2", inputs, outputs);

    double InputSum=0,OutputSum=0;
    cout<<endl;
    cout<<"Transaction Inputs: "<<endl;
    for(auto &input:inputs){
        string trans_id=input.GetTransactionId();
        int index=input.GetTransactionIndex();
        double amount=utxoMgr.GetUTXODetails(trans_id,index).first;
        InputSum+=amount;
        cout<<"Transaction id: "<<trans_id<<", Transaction index: "<<index<<", Amount: "<<fixed<<setprecision(4)<<amount<<endl;
    }

    cout<<endl;
    cout<<"Transaction Outputs: "<<endl;
    for(auto &output:outputs){
        string owner=output.GetTransactionOwner();
        double amount=output.GetTransactionAmount();
        cout<<"Owner: "<<owner<<", Amount: "<<fixed<<setprecision(4)<<amount<<endl;
        OutputSum+=amount;
    }
    cout<<endl;
    
    auto res = mempool.add_transaction(tx, utxoMgr);
    if(res.first){
        cout << "[PASS] Multi-input TX Accepted." << endl;
        cout<<"Transaction Fee: "<<fixed<<setprecision(4)<<InputSum-OutputSum<<endl;
    }
    else cout << "[FAIL] Multi-input TX Rejected." << endl;
}

void TestRunner::test3_DoubleSpendSameTX() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Attempting to spend (genesis, 0) twice in ONE transaction" << endl;
    cout<<endl;

    vector<TransactionInput> inputs = { 
        TransactionInput("tx_genesis_000", 0, "Alice"),
        TransactionInput("tx_genesis_000", 0, "Alice") // duplicate input
    };
    vector<TransactionOutput> outputs = { TransactionOutput(100.0, "Bob") };
    Transaction tx("tx3", inputs, outputs);

    auto res = mempool.add_transaction(tx, utxoMgr);
    if(!res.first) cout << "[PASS] Rejected: " << res.second << endl;
    else cout << "[FAIL] System accepted double spend!" << endl;
}

void TestRunner::test4_MempoolDoubleSpend() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: TX1 spends UTXO. TX2 tries to spend SAME UTXO." << endl;
    cout<<endl;

    // TX1: Alice -> Bob
    Transaction tx1("tx4a", {TransactionInput("tx_genesis_000", 0, "Alice")}, {TransactionOutput(10.0, "Bob"), TransactionOutput(39.0, "Alice")});
    mempool.add_transaction(tx1, utxoMgr);
    cout << " -> TX1 submitted (Valid)" << endl;
    cout<<endl;
    // TX2: Alice -> Charlie (Using same input)
    Transaction tx2("tx4b", {TransactionInput("tx_genesis_000", 0, "Alice")}, {TransactionOutput(10.0, "Charlie")});
    auto res = mempool.add_transaction(tx2, utxoMgr);

    if(!res.first) cout << "[PASS] TX2 Rejected: " << res.second << endl;
    else cout << "[FAIL] TX2 Accepted (Double Spend allowed!)" << endl;
}

void TestRunner::test5_InsufficientFunds() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Bob (has 30) tries to send 35" << endl;
    cout<<endl;

    Transaction tx("tx5", {TransactionInput("tx_genesis_000", 1, "Bob")}, {TransactionOutput(35.0, "Alice")});
    auto res = mempool.add_transaction(tx, utxoMgr);
    
    if(!res.first) cout << "[PASS] Rejected: " << res.second << endl;
    else cout << "[FAIL] Accepted insufficient funds." << endl;
}

void TestRunner::test6_NegativeAmount() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Output amount is -5.0" << endl;
    cout<<endl;

    Transaction tx("tx6", {TransactionInput("tx_genesis_000", 0, "Alice")}, {TransactionOutput(-5.0, "Bob")});
    auto res = mempool.add_transaction(tx, utxoMgr);

    if(!res.first) cout << "[PASS] Rejected: " << res.second << endl;
    else cout << "[FAIL] Accepted negative amount." << endl;
}

void TestRunner::test7_ZeroFee() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Input (5.0) == Output (5.0). Fee = 0." << endl;
    cout<<endl;

    Transaction tx("tx7", {TransactionInput("tx_genesis_000", 4, "Eve")}, {TransactionOutput(5.0, "Bob")});
    auto res = mempool.add_transaction(tx, utxoMgr);

    if(res.first) cout << "[PASS] Zero fee TX Accepted." << endl;
    else cout << "[FAIL] Zero fee TX Rejected." << endl;
}

void TestRunner::test8_RaceAttack() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Low Fee TX arrives first. High Fee TX arrives second (Same Input)." << endl;
    cout<<endl;

    vector<TransactionInput> in = { TransactionInput("tx_genesis_000", 0, "Alice") };
    
    // TX A (Low Fee)
    Transaction txA("tx8_low", in, { TransactionOutput(49.0, "Bob") }); // Fee 1.0
    mempool.add_transaction(txA, utxoMgr);
    cout << " -> TX A (First) submitted." << endl;
    cout<<endl;

    // TX B (High Fee)
    Transaction txB("tx8_high", in, { TransactionOutput(45.0, "Bob") }); // Fee 5.0
    auto res = mempool.add_transaction(txB, utxoMgr);

    if(!res.first) cout << "[PASS] TX B (Second) Rejected: " << res.second << endl;
    else cout << "[FAIL] TX B Replaced TX A (RBF not implemented/allowed here)." << endl;
}


void viewMempool(Mempool &mempool) {
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


void TestRunner::test9_MiningFlow() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    BlockChain blockchain; 
    cout << "Test: Add TX -> Mine Block -> Verify UTXO Update" << endl;
    cout<<endl;

    Transaction tx("tx9", {TransactionInput("tx_genesis_000", 0, "Alice")}, {TransactionOutput(10.0, "Bob"), TransactionOutput(39.999, "Alice")});
    mempool.add_transaction(tx, utxoMgr);

    cout<<"Mempool before mining: "<<endl;
    viewMempool(mempool);
    cout<<endl;

    blockchain.mine_block("Miner1", mempool, utxoMgr, mempool.getSpent_utxo()); 

    cout<<endl;
    cout<<"Mempool after mining: "<<endl;
    viewMempool(mempool);
    cout<<endl;

    cout<<"BlockChain after mining: "<<endl;
    blockchain.printBlockChain();
    cout<<endl;
    if(!utxoMgr.exists("tx_genesis_000", 0) && utxoMgr.exists("tx9", 0)) {
        cout << "[PASS] Mining updated UTXO set correctly." << endl;
    } else {
        cout << "[FAIL] UTXO set state incorrect after mining." << endl;
    }
}

void TestRunner::test10_UnconfirmedChain() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Spending an unconfirmed UTXO (Chained Mempool)" << endl;
    cout<<endl;

    Transaction tx1("tx10a", {TransactionInput("tx_genesis_000", 0, "Alice")}, {TransactionOutput(50.0, "Bob")});
    mempool.add_transaction(tx1, utxoMgr);

    Transaction tx2("tx10b", {TransactionInput("tx10a", 0, "Bob")}, {TransactionOutput(50.0, "Charlie")});
    
    auto res = mempool.add_transaction(tx2, utxoMgr);
    
    if(!res.first) cout << "[PASS] Unconfirmed spend Rejected " << endl;
    else cout << "[PASS/INFO] Unconfirmed spend Accepted ." << endl;
}