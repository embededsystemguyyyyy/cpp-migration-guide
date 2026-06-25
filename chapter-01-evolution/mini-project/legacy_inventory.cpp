// legacy_inventory.cpp
// C++03/C++11 style inventory system
// YOUR JOB: Find 5 legacy patterns and their modern replacements
// Compile: g++ -std=c++11 -o legacy_inventory legacy_inventory.cpp

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdio>

// Legacy pattern: struct with operator() instead of lambda
struct IsLowStock {
    int threshold;
    IsLowStock(int t) : threshold(t) {}
    bool operator()(const std::pair<std::string, int>& item) const {
        return item.second < threshold;
    }
};

// Legacy pattern: raw pointer ownership
struct Warehouse {
    std::string* name;
    std::vector<std::pair<std::string, int> >* inventory;

    Warehouse(const std::string& n) {
        name = new std::string(n);
        inventory = new std::vector<std::pair<std::string, int> >();
    }

    ~Warehouse() {
        delete name;
        delete inventory;
    }

    void addItem(const std::string item, int quantity) {
        inventory->push_back(std::make_pair(item, quantity));
    }
};

// Legacy pattern: takes string by value (unnecessary copy)
void printItem(std::string name, int quantity) {
    // Legacy pattern: printf instead of std::cout or std::print
    printf("  Item: %-20s Quantity: %d\n", name.c_str(), quantity);
}

int main() {
    Warehouse wh("Main Warehouse");
    wh.addItem("Bolts",       120);
    wh.addItem("Nuts",        5);
    wh.addItem("Washers",     200);
    wh.addItem("Screws",      3);
    wh.addItem("Anchors",     75);
    wh.addItem("Rivets",      8);

    printf("=== %s Inventory Report ===\n", wh.name->c_str());

    // Legacy pattern: explicit iterator syntax
    for (std::vector<std::pair<std::string, int> >::const_iterator it =
             wh.inventory->begin(); it != wh.inventory->end(); ++it) {
        printItem(it->first, it->second);
    }

    // Find low stock items using functor
    std::vector<std::pair<std::string, int> > lowStock;
    std::copy_if(wh.inventory->begin(), wh.inventory->end(),
                 std::back_inserter(lowStock), IsLowStock(10));

    printf("\n=== Low Stock Alert (< 10 units) ===\n");
    for (std::vector<std::pair<std::string, int> >::const_iterator it =
             lowStock.begin(); it != lowStock.end(); ++it) {
        printf("  WARNING: %s only has %d units left!\n",
               it->first.c_str(), it->second);
    }

    return 0;
}
