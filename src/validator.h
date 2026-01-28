#pragma once
#include <set>
#include <iostream>
#include "transaction.h"
#include "utxomanager.h"
using namespace std;

pair<bool,string> validateTransaction(Transaction& tx, UTXOManager& utxo_manager,double &InputSum,double &OutputSum,set<pair<string,int>>& spent_utxos);