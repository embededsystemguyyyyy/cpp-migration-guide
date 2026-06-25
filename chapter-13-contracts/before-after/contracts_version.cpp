// contracts_version.cpp — Same banking logic using C++26 Contracts
// Preconditions and postconditions are part of the function signature
// NOTE: C++26 contracts are experimental — check your compiler's support flags
// GCC:   g++ -std=c++26 -fcontracts -o contracts_version contracts_version.cpp
// Clang: clang++ -std=c++26 -fexperimental-library contracts_version.cpp

#include <iostream>
#include <print>

// With contracts, the function's requirements are visible at the call site
// The compiler can check them in debug mode and optimize them away in release

double withdraw(double balance, double amount)
    pre  (balance >= 0)          // caller must ensure this
    pre  (amount > 0)            // caller must ensure this
    pre  (amount <= balance)     // caller must ensure this
    post (result: result >= 0)   // we promise this about our return value
{
    return balance - amount;
    // No defensive if/throw needed — contracts handle it
}

double deposit(double balance, double amount)
    pre  (balance >= 0)
    pre  (amount > 0)
    post (result: result > balance)  // depositing always increases balance
{
    return balance + amount;
}

double applyInterest(double balance, double rate)
    pre  (balance >= 0)
    pre  (rate > -1.0)   // rate of -100% or worse makes no sense
    pre  (rate < 10.0)   // 1000% interest is probably a bug
    post (result: result >= 0)
{
    return balance * (1.0 + rate);
}

int main() {
    double balance = 1000.0;
    std::println("Starting balance: ${}", balance);

    balance = deposit(balance, 500.0);
    std::println("After deposit:    ${}", balance);

    balance = withdraw(balance, 200.0);
    std::println("After withdrawal: ${}", balance);

    balance = applyInterest(balance, 0.05);
    std::println("After interest:   ${:.2f}", balance);

    // This violates the precondition — caught immediately in debug mode
    // In release mode, the behavior is configurable (ignore / terminate / log)
    // balance = withdraw(balance, 99999.0);
    // Contract violation: 'amount <= balance' evaluated to false

    return 0;
}
