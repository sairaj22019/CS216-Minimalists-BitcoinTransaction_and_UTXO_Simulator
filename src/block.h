#pragma once
#include<vector>
#include<string>
#include "utxomanager.h"
#include "mempool.h"
#include "transaction.h"
using namespace std;

class Block{
    private:
    vector<Transaction> block;

    public:
    Block(vector<Transaction> txs);

    void add_transaction(Transaction t);
};

class BlockChain{
    private:
    vector<Block> blocks;

    public:
    BlockChain();
    
    void mine_block(string miner_address,Mempool &mempool,UTXOManager &utxo_manager,set<pair<string,int>> spent_utxos1,int num_txs = 5);  
};

