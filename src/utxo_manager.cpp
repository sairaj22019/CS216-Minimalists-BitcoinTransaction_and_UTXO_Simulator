#include <map>
#include <vector>
#include <iostream>
using namespace std;

class UTXOManager {
    private:

        map<pair<string,int>,pair<double,string>> utxo_set; // To store (tx_id, index) - > (amount, owner)

    public:

        UTXOManager() { 
            // Constructor
        }

        /// @brief Returns whether a Transaction ID exists in the UTXO Set
        /// @param tx_id The Transaction ID
        /// @param index The Index
        /// @return A boolean indicating whether the transaction exists.
        bool exists(string tx_id,int index) {
            if(utxo_set.find({tx_id,index})!=utxo_set.end()) {
                return true;
            }
            return false;
        }

        pair<double,string> GetUTXODetails(string tx_id,int index) {
            return utxo_set[{tx_id,index}];
        }

        /// @brief Adds a UTXO into the UTXO set
        /// @param tx_id The Transaction ID
        /// @param index The Index
        /// @param amount The amount of the UTXO
        /// @param owner The owner of the UTXO
        void add_utxo(string tx_id,int index, double amount, string owner) {
            if(exists(tx_id,index)) {
                cout<<"Transaction associated with transaction id: "<<tx_id<<" and index: "<<index<<" already exists"<<endl;
                return;
            }
            utxo_set[{tx_id,index}]={amount,owner};
        }

        /// @brief Removes a transaction from the UTXO set
        /// @param tx_id The Transaction ID of the UTXO
        /// @param index The Index of the UTXO
        void remove_utxo(string tx_id,int index) {
            if(!exists(tx_id,index)) {
                cout<<"Transaction associated with transaction id: "<<tx_id<<" and index: "<<index<<" does not exist"<<endl;
                return;
            }
            utxo_set.erase({tx_id,index});
        }

        /// @brief Returns the balance of an owner.
        /// @param owner Name of the owner.
        /// @return The balance amount.
        double get_balance(string owner) { 
            double balance = 0;
            for(auto &utxo: utxo_set) {
                string tx_id = utxo.first.first;
                int index = utxo.first.second;
                double amount = utxo.second.first;
                string utxo_owner = utxo.second.second;

                if(utxo_owner == owner) {
                    balance += amount;
                }
            }
            return balance;
        }

        /// @brief Returns a UTXO list of the specified owner.
        /// @param owner Name of the owner.
        /// @return The list containing all the UTXO's of the owner.
        vector<pair<pair<string,double>,pair<int,string>>> get_utxos_for_owner(string owner) {
            vector<pair<pair<string,double>,pair<int,string>>> utxo_list;
            for(auto &utxo: utxo_set) {
                string tx_id = utxo.first.first;
                int index = utxo.first.second;
                double amount = utxo.second.first;
                string utxo_owner = utxo.second.second;

                if(utxo_owner == owner) {
                    utxo_list.push_back({{utxo_owner,amount},{index,tx_id}});
                }
            }
            return utxo_list;
        }

};