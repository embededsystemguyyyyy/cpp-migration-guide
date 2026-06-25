// ranges_version.cpp — Same logic as raw_loop_version.cpp using C++20 Ranges
// Compile: g++ -std=c++20 -o ranges_version ranges_version.cpp

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
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

    // The entire pipeline in one expression — reads like a description
    auto pipeline = products
        | std::views::filter([](const Product& p) {
            return p.category == "gadgets" && p.stock > 0;
          })
        | std::views::transform([](Product p) {
            p.price *= 0.9; // 10% discount
            return p;
          });

    // Materialize into a vector for sorting (ranges::sort needs random access)
    std::vector<Product> discounted(pipeline.begin(), pipeline.end());
    std::ranges::sort(discounted, {}, &Product::price);

    // Extract names lazily — no intermediate vector allocated
    auto names = discounted | std::views::transform(&Product::name);

    std::cout << "In-stock gadgets (10% off, sorted by price):\n";
    for (const auto& name : names) {
        std::cout << "  " << name << "\n";
    }

    // Total inventory value
    double total = std::accumulate(discounted.begin(), discounted.end(), 0.0,
        [](double sum, const Product& p) { return sum + p.price * p.stock; });

    std::cout << "Total inventory value: $" << total << "\n";

    return 0;
}
