#include <bits/stdc++.h>
using namespace std;

class UTXOManager {
    private:

        map<pair<string,int>,pair<double,string>> utxo_set; // To store (tx_id, index) - > (amount, owner)

    public:

        UTXOManager() { 
            // Constructor
        }

        void add_utxo(string tx_id,int index, double amount, string owner) {
            
        }

        void remove_utxo(string tx_id,int index) {

        }

        double get_balance(string owner) { 

        }

        bool exists(string tx_id,int index) {

        }

        vector<pair<string,pair<int,double>>> get_utxos_for_owner(string owner) {
            
        }

};