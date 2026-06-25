// transaction_processor.cpp
// Legacy C++17 financial transaction processor
// This is the codebase you will migrate throughout the book
// It works. It compiles. It has problems.
// Compile: g++ -std=c++17 -o transaction_processor transaction_processor.cpp

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ── Domain Types ─────────────────────────────────────────────────────────

enum TransactionType { DEPOSIT, WITHDRAWAL, TRANSFER };

struct Transaction {
    int             id;
    TransactionType type;
    double          amount;
    std::string     accountFrom;
    std::string     accountTo;
    bool            processed;
    std::string     errorMessage;
};

struct Account {
    std::string id;
    std::string owner;
    double      balance;
    bool        frozen;
};

// ── Legacy: functor instead of lambda ────────────────────────────────────

struct IsLargeTransaction {
    double threshold;
    explicit IsLargeTransaction(double t) : threshold(t) {}
    bool operator()(const Transaction& t) const {
        return t.amount > threshold;
    }
};

struct ByAmount {
    bool operator()(const Transaction& a, const Transaction& b) const {
        return a.amount < b.amount;
    }
};

// ── Account Repository — raw pointer, no RAII ─────────────────────────

class AccountRepository {
    std::vector<Account>* accounts;  // raw owning pointer — ask why

public:
    AccountRepository() {
        accounts = new std::vector<Account>();
        // Seed with test data
        accounts->push_back({"ACC001", "Alice",   5000.0, false});
        accounts->push_back({"ACC002", "Bob",     1200.0, false});
        accounts->push_back({"ACC003", "Charlie", 300.0,  false});
        accounts->push_back({"ACC004", "Diana",   8750.0, true });  // frozen
    }

    ~AccountRepository() {
        delete accounts;
    }

    Account* find(const std::string& id) {
        for (auto& acc : *accounts) {
            if (acc.id == id) return &acc;
        }
        return nullptr;
    }

    void updateBalance(const std::string& id, double newBalance) {
        Account* acc = find(id);
        if (!acc) throw std::runtime_error("Account not found: " + id);
        acc->balance = newBalance;
    }
};

// ── Transaction Processor — exceptions for control flow ───────────────

class TransactionProcessor {
    AccountRepository* repo;  // another raw pointer
    std::vector<Transaction> processedLog;

public:
    explicit TransactionProcessor(AccountRepository* r) : repo(r) {}

    // Returns true on success — caller must check errorMessage for details
    bool process(Transaction& tx) {
        try {
            validateTransaction(tx);

            if (tx.type == DEPOSIT) {
                Account* acc = repo->find(tx.accountTo);
                repo->updateBalance(tx.accountTo, acc->balance + tx.amount);

            } else if (tx.type == WITHDRAWAL) {
                Account* acc = repo->find(tx.accountFrom);
                repo->updateBalance(tx.accountFrom, acc->balance - tx.amount);

            } else if (tx.type == TRANSFER) {
                Account* from = repo->find(tx.accountFrom);
                Account* to   = repo->find(tx.accountTo);
                repo->updateBalance(tx.accountFrom, from->balance - tx.amount);
                repo->updateBalance(tx.accountTo,   to->balance   + tx.amount);
            }

            tx.processed = true;
            processedLog.push_back(tx);
            return true;

        } catch (const std::exception& e) {
            tx.processed    = false;
            tx.errorMessage = e.what();
            return false;
        }
    }

    // Legacy: printf for reporting
    void printReport() const {
        printf("\n=== Transaction Report ===\n");
        printf("Processed: %zu transactions\n\n", processedLog.size());

        for (std::vector<Transaction>::const_iterator it = processedLog.begin();
             it != processedLog.end(); ++it) {
            printf("  TX #%d | Amount: $%.2f | %s\n",
                   it->id, it->amount,
                   it->processed ? "SUCCESS" : "FAILED");
        }

        // Find large transactions with functor
        std::vector<Transaction> large;
        std::copy_if(processedLog.begin(), processedLog.end(),
                     std::back_inserter(large), IsLargeTransaction(1000.0));

        printf("\nLarge transactions (>$1000): %zu\n", large.size());
        for (const auto& tx : large) {
            printf("  TX #%d: $%.2f\n", tx.id, tx.amount);
        }
    }

private:
    void validateTransaction(const Transaction& tx) {
        if (tx.amount <= 0)
            throw std::invalid_argument("Amount must be positive");

        if (tx.type == WITHDRAWAL || tx.type == TRANSFER) {
            Account* from = repo->find(tx.accountFrom);
            if (!from)
                throw std::runtime_error("Source account not found: " + tx.accountFrom);
            if (from->frozen)
                throw std::runtime_error("Account is frozen: " + tx.accountFrom);
            if (from->balance < tx.amount)
                throw std::runtime_error("Insufficient funds in: " + tx.accountFrom);
        }

        if (tx.type == DEPOSIT || tx.type == TRANSFER) {
            Account* to = repo->find(tx.accountTo);
            if (!to)
                throw std::runtime_error("Destination account not found: " + tx.accountTo);
            if (to->frozen)
                throw std::runtime_error("Destination account is frozen: " + tx.accountTo);
        }
    }
};

// ── Main ─────────────────────────────────────────────────────────────────

int main() {
    AccountRepository repo;
    TransactionProcessor processor(&repo);

    std::vector<Transaction> transactions = {
        {1, DEPOSIT,    500.0,  "",       "ACC001", false, ""},
        {2, WITHDRAWAL, 200.0,  "ACC002", "",       false, ""},
        {3, TRANSFER,   1500.0, "ACC001", "ACC003", false, ""},
        {4, WITHDRAWAL, 99999,  "ACC002", "",       false, ""},  // should fail
        {5, DEPOSIT,    100.0,  "",       "ACC004", false, ""},  // frozen account
        {6, TRANSFER,   250.0,  "ACC003", "ACC002", false, ""},
    };

    printf("Processing %zu transactions...\n", transactions.size());

    for (auto& tx : transactions) {
        bool ok = processor.process(tx);
        if (!ok) {
            fprintf(stderr, "Transaction #%d FAILED: %s\n",
                    tx.id, tx.errorMessage.c_str());
        }
    }

    processor.printReport();

    // Check final balances
    printf("\n=== Final Balances ===\n");
    for (const std::string& id : {"ACC001", "ACC002", "ACC003", "ACC004"}) {
        Account* acc = repo.find(id);
        if (acc) printf("  %s (%s): $%.2f%s\n",
                        acc->id.c_str(), acc->owner.c_str(), acc->balance,
                        acc->frozen ? " [FROZEN]" : "");
    }

    return 0;
}
