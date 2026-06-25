// main.cpp — C++23 starter project
// If this compiles and prints, your toolchain is ready
#include <print>
#include <vector>
#include <ranges>
#include <expected>

// Test std::expected (C++23)
std::expected<int, std::string> divide(int a, int b) {
    if (b == 0) return std::unexpected("Division by zero");
    return a / b;
}

int main() {
    // Test std::print (C++23)
    std::println("C++23 is ready. Let's migrate.");

    // Test std::ranges (C++20)
    std::vector<int> nums = {1, 2, 3, 4, 5, 6};
    std::print("Even numbers: ");
    for (int n : nums | std::views::filter([](int n){ return n % 2 == 0; })) {
        std::print("{} ", n);
    }
    std::println("");

    // Test std::expected (C++23)
    auto result = divide(10, 2);
    if (result) std::println("10 / 2 = {}", *result);

    auto err = divide(10, 0);
    if (!err) std::println("Error: {}", err.error());

    return 0;
}
