#pragma once
#include <map>
#include <string>
#include <vector>
using namespace std;

class UTXOManager {
    private:

        map<pair<string,int>,pair<double,string>> utxo_set; // To store (tx_id, index) - > (amount, owner)

    public:

        UTXOManager();

        /// @brief Returns whether a Transaction ID exists in the UTXO Set
        /// @param tx_id The Transaction ID
        /// @param index The Index
        /// @return A boolean indicating whether the transaction exists.
        bool exists(string tx_id,int index);

        pair<double,string> GetUTXODetails(string tx_id,int index);

        /// @brief Adds a UTXO into the UTXO set
        /// @param tx_id The Transaction ID
        /// @param index The Index
        /// @param amount The amount of the UTXO
        /// @param owner The owner of the UTXO
        void add_utxo(string tx_id,int index, double amount, string owner);

        /// @brief Removes a transaction from the UTXO set
        /// @param tx_id The Transaction ID of the UTXO
        /// @param index The Index of the UTXO
        void remove_utxo(string tx_id,int index);

        /// @brief Returns the balance of an owner.
        /// @param owner Name of the owner.
        /// @return The balance amount.
        double get_balance(string owner);

        /// @brief Returns a UTXO list of the specified owner.
        /// @param owner Name of the owner.
        /// @return The list containing all the UTXO's of the owner.
        vector<pair<pair<string,double>,pair<int,string>>> get_utxos_for_owner(string owner);

};