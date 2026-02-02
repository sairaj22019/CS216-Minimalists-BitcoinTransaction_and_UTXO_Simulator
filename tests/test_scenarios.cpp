#include <iostream>
#include <vector>
#include "tests.h"
#include "../src/transaction.h"
#include "../src/utxomanager.h"
#include "../src/mempool.h"
#include "../src/block.h" 

using namespace std;

void setupTestEnv(UTXOManager& utxoMgr, Mempool& mempool) {
    mempool.clear();
    utxoMgr.add_utxo("genesis_tx", 0, 50.0, "Alice");
    utxoMgr.add_utxo("genesis_tx", 1, 30.0, "Bob");
    utxoMgr.add_utxo("genesis_tx", 2, 20.0, "Charlie");
    utxoMgr.add_utxo("genesis_tx", 3, 10.0, "David");
    utxoMgr.add_utxo("genesis_tx", 4, 5.0, "Eve");
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
    
    vector<TransactionInput> inputs = { TransactionInput("genesis_tx", 0, "Alice") };
    vector<TransactionOutput> outputs = { 
        TransactionOutput(10.0, "Bob"), 
        TransactionOutput(39.999, "Alice") 
    };
    
    Transaction tx("tx1", inputs, outputs);
    
    auto res = mempool.add_transaction(tx, utxoMgr);
    if(res.first) cout << "[PASS] Transaction Accepted." << endl;
    else cout << "[FAIL] Transaction Rejected: " << res.second << endl;
}

void TestRunner::test2_MultipleInputs() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    utxoMgr.add_utxo("extra_tx", 0, 20.0, "Alice");

    cout << "Test: Alice spends two UTXOs (50 + 20) to send 60" << endl;

    vector<TransactionInput> inputs = { 
        TransactionInput("genesis_tx", 0, "Alice"),
        TransactionInput("extra_tx", 0, "Alice")
    };
    vector<TransactionOutput> outputs = { 
        TransactionOutput(60.0, "Bob"),
        TransactionOutput(9.9, "Alice") 
    };
    Transaction tx("tx2", inputs, outputs);

    auto res = mempool.add_transaction(tx, utxoMgr);
    if(res.first) cout << "[PASS] Multi-input TX Accepted." << endl;
    else cout << "[FAIL] Multi-input TX Rejected." << endl;
}

void TestRunner::test3_DoubleSpendSameTX() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Attempting to spend (genesis, 0) twice in ONE transaction" << endl;

    vector<TransactionInput> inputs = { 
        TransactionInput("genesis_tx", 0, "Alice"),
        TransactionInput("genesis_tx", 0, "Alice") // duplicate input
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

    // TX1: Alice -> Bob
    Transaction tx1("tx4a", {TransactionInput("genesis_tx", 0, "Alice")}, {TransactionOutput(10.0, "Bob"), TransactionOutput(39.0, "Alice")});
    mempool.add_transaction(tx1, utxoMgr);
    cout << " -> TX1 submitted (Valid)" << endl;

    // TX2: Alice -> Charlie (Using same input)
    Transaction tx2("tx4b", {TransactionInput("genesis_tx", 0, "Alice")}, {TransactionOutput(10.0, "Charlie")});
    auto res = mempool.add_transaction(tx2, utxoMgr);

    if(!res.first) cout << "[PASS] TX2 Rejected: " << res.second << endl;
    else cout << "[FAIL] TX2 Accepted (Double Spend allowed!)" << endl;
}

void TestRunner::test5_InsufficientFunds() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Bob (has 30) tries to send 35" << endl;

    Transaction tx("tx5", {TransactionInput("genesis_tx", 1, "Bob")}, {TransactionOutput(35.0, "Alice")});
    auto res = mempool.add_transaction(tx, utxoMgr);
    
    if(!res.first) cout << "[PASS] Rejected: " << res.second << endl;
    else cout << "[FAIL] Accepted insufficient funds." << endl;
}

void TestRunner::test6_NegativeAmount() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Output amount is -5.0" << endl;

    Transaction tx("tx6", {TransactionInput("genesis_tx", 0, "Alice")}, {TransactionOutput(-5.0, "Bob")});
    auto res = mempool.add_transaction(tx, utxoMgr);

    if(!res.first) cout << "[PASS] Rejected: " << res.second << endl;
    else cout << "[FAIL] Accepted negative amount." << endl;
}

void TestRunner::test7_ZeroFee() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Input (5.0) == Output (5.0). Fee = 0." << endl;

    Transaction tx("tx7", {TransactionInput("genesis_tx", 4, "Eve")}, {TransactionOutput(5.0, "Bob")});
    auto res = mempool.add_transaction(tx, utxoMgr);

    if(res.first) cout << "[PASS] Zero fee TX Accepted." << endl;
    else cout << "[FAIL] Zero fee TX Rejected." << endl;
}

void TestRunner::test8_RaceAttack() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Low Fee TX arrives first. High Fee TX arrives second (Same Input)." << endl;

    vector<TransactionInput> in = { TransactionInput("genesis_tx", 0, "Alice") };
    
    // TX A (Low Fee)
    Transaction txA("tx8_low", in, { TransactionOutput(49.0, "Bob") }); // Fee 1.0
    mempool.add_transaction(txA, utxoMgr);
    cout << " -> TX A (First) submitted." << endl;

    // TX B (High Fee)
    Transaction txB("tx8_high", in, { TransactionOutput(45.0, "Bob") }); // Fee 5.0
    auto res = mempool.add_transaction(txB, utxoMgr);

    if(!res.first) cout << "[PASS] TX B (Second) Rejected: " << res.second << endl;
    else cout << "[FAIL] TX B Replaced TX A (RBF not implemented/allowed here)." << endl;
}

void TestRunner::test9_MiningFlow() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    BlockChain blockchain; 
    cout << "Test: Add TX -> Mine Block -> Verify UTXO Update" << endl;

    Transaction tx("tx9", {TransactionInput("genesis_tx", 0, "Alice")}, {TransactionOutput(10.0, "Bob"), TransactionOutput(39.0, "Alice")});
    mempool.add_transaction(tx, utxoMgr);


    blockchain.mine_block("Miner1", mempool, utxoMgr, mempool.getSpent_utxo()); 

    if(!utxoMgr.exists("genesis_tx", 0) && utxoMgr.exists("tx9", 0)) {
        cout << "[PASS] Mining updated UTXO set correctly." << endl;
    } else {
        cout << "[FAIL] UTXO set state incorrect after mining." << endl;
    }
}

void TestRunner::test10_UnconfirmedChain() {
    UTXOManager utxoMgr; Mempool mempool; setupTestEnv(utxoMgr, mempool);
    cout << "Test: Spending an unconfirmed UTXO (Chained Mempool)" << endl;

    Transaction tx1("tx10a", {TransactionInput("genesis_tx", 0, "Alice")}, {TransactionOutput(50.0, "Bob")});
    mempool.add_transaction(tx1, utxoMgr);

    Transaction tx2("tx10b", {TransactionInput("tx10a", 0, "Bob")}, {TransactionOutput(50.0, "Charlie")});
    
    auto res = mempool.add_transaction(tx2, utxoMgr);
    
    if(!res.first) cout << "[PASS] Unconfirmed spend Rejected (Policy: Reject)." << endl;
    else cout << "[PASS/INFO] Unconfirmed spend Accepted (Policy: Chain Support)." << endl;
}