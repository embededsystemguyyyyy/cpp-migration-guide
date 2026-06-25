// raw_loop_version.cpp — Data processing the old way
// Compile: g++ -std=c++17 -o raw_loop_version raw_loop_version.cpp

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Product {
    std::string name;
    double      price;
    int         stock;
    std::string category;
};

int main() {
    std::vector<Product> products = {
        {"Widget A",  9.99,  100, "widgets"},
        {"Widget B",  4.99,    5, "widgets"},
        {"Gadget X", 49.99,   50, "gadgets"},
        {"Gadget Y", 19.99,    0, "gadgets"},
        {"Gadget Z", 99.99,   20, "gadgets"},
        {"Doohickey", 2.99,  200, "misc"   },
    };

    // Step 1: filter to in-stock gadgets only
    std::vector<Product> inStockGadgets;
    for (const auto& p : products) {
        if (p.category == "gadgets" && p.stock > 0) {
            inStockGadgets.push_back(p);
        }
    }

    // Step 2: apply 10% discount
    std::vector<Product> discounted;
    for (auto p : inStockGadgets) {
        p.price *= 0.9;
        discounted.push_back(p);
    }

    // Step 3: sort by price ascending
    std::sort(discounted.begin(), discounted.end(),
        [](const Product& a, const Product& b) {
            return a.price < b.price;
        });

    // Step 4: extract names
    std::vector<std::string> names;
    for (const auto& p : discounted) {
        names.push_back(p.name);
    }

    // Step 5: print results
    std::cout << "In-stock gadgets (10% off, sorted by price):\n";
    for (const auto& name : names) {
        std::cout << "  " << name << "\n";
    }

    // Step 6: total value of in-stock gadgets after discount
    double total = 0.0;
    for (const auto& p : discounted) {
        total += p.price * p.stock;
    }
    std::cout << "Total inventory value: $" << total << "\n";

    return 0;
}
