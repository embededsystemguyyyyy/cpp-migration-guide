// manual_validation_version.cpp — Preconditions enforced manually with if/throw
// Compile: g++ -std=c++17 -o manual_validation_version manual_validation_version.cpp

#include <iostream>
#include <stdexcept>
#include <string>

// Every function has to manually write its own guard clauses
// Nothing enforces that callers respect the invariants

double withdraw(double balance, double amount) {
    // Preconditions — scattered, inconsistent, easy to forget
    if (balance < 0)
        throw std::invalid_argument("Balance cannot be negative");
    if (amount <= 0)
        throw std::invalid_argument("Withdrawal amount must be positive");
    if (amount > balance)
        throw std::invalid_argument("Insufficient funds");

    double new_balance = balance - amount;

    // Postcondition check — manually added, often forgotten
    if (new_balance < 0)
        throw std::logic_error("Postcondition violated: balance went negative");

    return new_balance;
}

double deposit(double balance, double amount) {
    if (balance < 0)
        throw std::invalid_argument("Balance cannot be negative");
    if (amount <= 0)
        throw std::invalid_argument("Deposit amount must be positive");

    return balance + amount;
}

double applyInterest(double balance, double rate) {
    // Forgot to validate rate — silent bug waiting to happen
    return balance * (1.0 + rate);
}

int main() {
    double balance = 1000.0;
    std::cout << "Starting balance: $" << balance << "\n";

    try {
        balance = deposit(balance, 500.0);
        std::cout << "After deposit:    $" << balance << "\n";

        balance = withdraw(balance, 200.0);
        std::cout << "After withdrawal: $" << balance << "\n";

        balance = applyInterest(balance, 0.05);
        std::cout << "After interest:   $" << balance << "\n";

        // This throws — but only at runtime
        balance = withdraw(balance, 99999.0);

    } catch (const std::exception& e) {
        std::cerr << "Transaction failed: " << e.what() << "\n";
    }

    return 0;
}
