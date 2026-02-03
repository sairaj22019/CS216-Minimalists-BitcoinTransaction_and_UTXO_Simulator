# CS216-Minimalists-BitcoinTransaction_and_UTXO_Simulator

## Team Name: Minimalists
### Team Members:
| Name                               | Roll Number |
|------------------------------------|-------------|
| Sairaj Srigiri                    | 240001070   |
| Kushal Gunala                     | 240001033   |
| Manish Donthula                   | 240001029   |
| Mohana Sarath Chandra Jandhyala    | 240041020   |

---

### Project Overview
This project is a C++ implementation of a Bitcoin Transaction and UTXO (Unspent Transaction Output) Simulator. It models the core logic of Bitcoin's transaction lifecycle, including UTXO management, transaction validation, mempool management, and the mining process. The simulation runs locally via a Command Line Interface (CLI).

---

### Prerequisites & Dependencies
* **Operating System:** Windows, macOS, or Linux.
* **Compiler:** A C++ compiler supporting C++11 or higher (e.g., `g++`, `clang++`).
* **Libraries:** Standard C++ libraries only (`<vector>`, `<map>`, `<set>`, `<iostream>`, etc.). No external dependencies are required.

---

### Installation & Execution Instructions

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/sairaj22019/CS216-Minimalists-BitcoinTransaction_and_UTXO_Simulator.git
    cd CS216-Minimalists-BitcoinTransaction_and_UTXO_Simulator
    ```

2.  **Compile the Project:**
    Navigate to the root directory of the project and run the following command to compile the source code and tests together:
    ```bash
    g++ src/*.cpp tests/*.cpp -o utxo_simulator
    ```
    *Note: Ensure your compiler can resolve the relative include paths provided in the source code.*

3.  **Run the Simulator:**
    ```bash
    ./utxo_simulator
    ```
    *(On Windows, run `utxo_simulator.exe`)*

---

### Design & Implementation Details

#### 1. UTXO Manager (`utxomanager.cpp`)
* **Storage:** We utilize a `std::map` to store the UTXO set for efficient lookups.
    * **Key:** `pair<string, int>` representing `(Transaction ID, Index)`.
    * **Value:** `pair<double, string>` representing `(Amount, Owner)`.
* **Efficiency:** This allows for $O(\log n)$ time complexity when checking if a UTXO exists or retrieving its details.
* We did not use an unordered map due to unavailability of standard hashing of pairs.
```
UTXOManager
├── utxo_set (std::map)
│   ├── Key: <TxID, Index>
│   └── Value: <Amount, Owner>
└── Methods
    ├── add_utxo() / remove_utxo()
    └── get_balance() / exists()
    └── get_utxos_from_owner()
```

#### 2. Transaction Validation (`validator.cpp`)
Before a transaction enters the mempool, it undergoes strict validation:
* **Existence:** Checks if referenced inputs exist in the UTXO set.
* **Ownership:** Verifies the spender owns the UTXO.
* **Double-Spend (Internal):** Ensures the same UTXO is not used twice within the same transaction inputs.
* **Double-Spend (Mempool):** Checks against `spent_utxos` to ensure a UTXO currently pending in the mempool isn't spent again.
* **Amounts:** Verifies `Input Sum >= Output Sum` and no negative outputs.
```
Validator (Module)
└── validateTransaction()
    ├── Inputs: Transaction, UTXOManager, Mempool
    ├── Checks: UTXO Existence, Ownership, Fees
    └── Returns: <IsValid (bool), Message (string)>
```

#### 3. Transaction Structure (`transaction.cpp`)
This file defines the core data structures used to represent value transfer within the simulator.
* **Classes:** `Transaction`, `TransactionInput`, and `TransactionOutput`.
* **Composition:** A `Transaction` object is a wrapper containing a unique ID (`tx_id`), a vector of inputs, and a vector of outputs.
 ```
  Transaction
├── tx_id (String)
├── Inputs (Vector)
│   └── TransactionInput <TxID, Index, Owner>
└── Outputs (Vector)
    └── TransactionOutput <Amount, Owner>
 ```

#### 4. Mempool Management (`mempool.cpp`)
* **Structure:** Stores valid transactions in a `vector`.
* **Conflict Prevention:** Maintains a separate `std::set<pair<string,int>> spent_utxos` to track UTXOs consumed by unconfirmed transactions(the transactions in the mempool that are not yet mined).
* **Ordering & Eviction:**
    * Transactions are stored as pairs: `(Fee, Transaction Object)`.
    * When the mempool reaches capacity (default 10), the system sorts transactions by fee (descending) and evicts the transaction with the **lowest fee** to make room for new ones.
```
Mempool
├── transactions (Vector)
│   └── Pair: <Fee, Transaction Object>
├── spent_utxos (Set)
│   └── <TxID, Index> (Locked inputs)
└── Methods
    ├── add_transaction() (Validates & Sorts)
    └── get_and_pop_top_transactions()
```

#### 5. Mining (`block.cpp`)
* **Selection:** The miner selects the top 5 transactions (if available) with the highest fees from the mempool.
* **Coinbase:** A coinbase transaction is created generating a reward of **3.125 BTC** plus the sum of transaction fees.
* **Update:** Upon mining, input UTXOs are removed from the global `UTXOManager`, and new output UTXOs are added.
```
BlockChain
├── blocks (Vector of Block objects)
└── Methods
    └── mine_block()
        ├── Select Top Txs (by Fee)
        ├── Update UTXOManager (Remove Inputs/Add Outputs)
        └── Create Coinbase Tx
```
---

### Design Decisions (FAQ Responses)

As per the assignment requirements, here are the specific design choices we made:

1.  **Transaction Ordering (Latency vs Fee):**
    * We implemented a **Fee-Based** ordering system. The mempool sorts transactions by the difference between inputs and outputs (the fee). Miners prioritize the transactions that pay the highest fees.

2.  **Unconfirmed Chain Handling (Test 10):**
    * **Decision:** **Reject.**
    * **Reasoning:** Our validator checks the global `UTXOManager` for input existence. Since the `UTXOManager` is only updated *after* a block is mined, a transaction trying to spend an output from a pending (unconfirmed) transaction will be rejected as "UTXO invalid" or "does not exist." This simplifies the logic and prevents complex dependency tracking in the mempool.

3.  **Mempool Eviction Policy:**
    * When the mempool is full, we sort the transactions by fee and remove the transaction with the **lowest fee**.

---

### Test Scenarios
The simulator includes 10 automated test scenarios (accessible via Option 5 in the Main Menu) covering:
1.  Basic Valid Transaction.
2.  Multiple Inputs (Aggregation).
3.  Double-Spend attempt in the same transaction.
4.  Mempool Double-Spend (Conflict detection).
5.  Insufficient Funds.
6.  Negative Output Amounts.
7.  Zero Fee Transaction.
8.  Race Attack Simulation (First-seen rule).
9.  Complete Mining Flow (UTXO updates).
10. Unconfirmed Chain attempts (Policy: Reject).
