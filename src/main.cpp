#include <iostream>
#include <map>
#include "utxomanager.h"
#include "mempool.h"
#include <iomanip>

using namespace std;

void generateGenesisBlock(UTXOManager& utxo_manager) {
    map<string,double> users = {
        {"Alice", 40.0},
        {"Bob", 50.0},
        {"Charlie", 70.0},
        {"Delta", 20.0},
        {"Eve", 10.0}
    };
    cout<<"--- Initial Genesis Block ---"<<endl;
    cout<<endl;
    int index = 0;
    for(auto user:users) {
        string tx_id = "tx_genesis_000";
        utxo_manager.add_utxo(tx_id,index,user.second,user.first);
        index++;
        cout<<"- "<<user.first<<" : "<<fixed<<setprecision(3)<<user.second<<" BTC"<<endl;
    }
}

void printChoices() {
    cout<<"Main Menu: \n"<<endl;
    cout<<"1. Create new transaction "<<endl;
    cout<<"2. View UTXO set "<<endl;
    cout<<"3. View mempool "<<endl;
    cout<<"4. Mine block "<<endl;
    cout<<"5. Run test scenarios "<<endl;
    cout<<"6. Exit "<<endl;
}

int main() {
    cout<<"========== BLOCKCHAIN TRANSACTION SIMULATOR =========="<<endl;
    cout<<"\n"<<endl;

    UTXOManager utxo_manager;
    generateGenesisBlock(utxo_manager);

    while(true) {
        cout<<"\n"<<endl;
        
        printChoices();
        cout<<"\n";
        cout<<"==== Enter Input Choice ===\n"<<endl;

        char s;
        cin>>s;

        if(!(('0'<=s)&&(s<='9'))) {
            cout<<"Please enter a digit in range of the given choices."<<endl;
            continue;
        }

        int n = s-'0';
        switch(n) {
            case 1:
                cout<<1<<endl;
                break;
            case 2:
                cout<<2<<endl;
                break;
            case 3:
                cout<<3<<endl;
                break;
            case 4:
                cout<<4<<endl;
                break;
            case 5:
                cout<<5<<endl;
                break;
            case 6:
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