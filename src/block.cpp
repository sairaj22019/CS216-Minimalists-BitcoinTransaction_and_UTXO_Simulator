#include<vector>
#include<string>
#include<iomanip>
#include "utxomanager.h"
#include "mempool.h"
#include "transaction.h"
#include "block.h"
using namespace std;


Block::Block(vector<Transaction> txs)
{
    block = txs;
}

void Block::add_transaction(Transaction t)
{
    block.push_back(t);
}

void Block::printBlock(){
    cout<<block.back().getTransactionID()<<endl<<endl;

    for(int i=0; i+1<block.size(); i++) {
        cout<<block[i].getTransactionID()<<endl;
    }
}


BlockChain::BlockChain(){
    blocks={};
}

void BlockChain::mine_block(string miner_address,Mempool &mempool,UTXOManager &utxo_manager,set<pair<string,int>> spent_utxos1,int num_txs)
{
    vector<Transaction> top_txs = mempool.get_top_and_pop_transactions(num_txs);

    Block block(top_txs);
    double InputSum = 0,OutputSum = 0;
    for(auto txs : top_txs)
    {
        string tx_id = txs.getTransactionID();
        vector<TransactionInput> inputs = txs.getTransactionInputs();
        vector<TransactionOutput> outputs = txs.getTransactionOutputs();
        for(auto input : inputs)
        {
            string tx_id1 = input.GetTransactionId();
            int index = input.GetTransactionIndex();
            double inputamount = utxo_manager.GetUTXODetails(tx_id1,index).first;
            InputSum+=inputamount;
            spent_utxos1.erase({tx_id1,index});
            utxo_manager.remove_utxo(tx_id1,index);
        }
        int idx=0;
        for(auto output : outputs)
        {
            double amount = output.GetTransactionAmount();
            string owner = output.GetTransactionOwner();
            OutputSum+=amount;
            utxo_manager.add_utxo(tx_id,idx,amount,owner);
            idx++;
        }
    }
    mempool.updateSpent_utxo(spent_utxos1);
    double transaction_fee = InputSum - OutputSum;
    if(transaction_fee < 0) 
    {
        cout<<"Invalid block as Output sum is greater than input sum"<<endl;
        return;
    }

    cout<<"Total transaction fee : "<<fixed<<setprecision(4)<<transaction_fee<<" BTC"<<endl;

    
    double mining_reward = 3.125;
    string block_id="coinbase_tx_";
    block_id+=to_string(blocks.size());

    utxo_manager.add_utxo(block_id,0,transaction_fee+mining_reward,miner_address);
    TransactionOutput coin_base_output(transaction_fee+mining_reward,miner_address);
    Transaction coin_base_tx(block_id,{},{coin_base_output});

    cout<<"Miner "<<miner_address<<" received total reward of "<<fixed<<setprecision(4)<<transaction_fee+mining_reward<<" BTC"<<endl;

    block.add_transaction(coin_base_tx);
    blocks.push_back(block);
}

void BlockChain::printBlockChain(){
    cout<<endl;
    cout<<"Printing Blockchain..."<<endl<<endl;
    if(blocks.empty()){
        cout<<"Blockchain is empty"<<endl;
        return;
    }
    for(int i=0;i<blocks.size();i++){
        cout<<"---------- Block "<<i+1<<" ----------"<<endl;
        blocks[i].printBlock();
        cout<<"-----------------------------"<<endl;
    }
}