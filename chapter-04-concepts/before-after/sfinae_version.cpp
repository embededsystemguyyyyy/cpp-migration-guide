// sfinae_version.cpp — The painful legacy way
// Template constraints via SFINAE: works but unreadable
// Compile: g++ -std=c++17 -o sfinae_version sfinae_version.cpp

#include <iostream>
#include <type_traits>
#include <vector>
#include <string>

// SFINAE: enable this function only for arithmetic types
// Good luck reading this in 6 months
template <typename T,
          typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T add(T a, T b) {
    return a + b;
}

// SFINAE: only for types that have a .size() method
// The error message if you pass the wrong type? Enjoy 80 lines of template noise
template <typename Container,
          typename = std::enable_if_t<
              std::is_same_v<
                  decltype(std::declval<Container>().size()),
                  typename Container::size_type>>>
auto getSize(const Container& c) -> typename Container::size_type {
    return c.size();
}

// SFINAE: only for floating point — try reading this at 2am
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, T>
square(T x) {
    return x * x;
}

int main() {
    std::cout << "add(3, 4)       = " << add(3, 4)         << "\n";
    std::cout << "add(1.5, 2.5)   = " << add(1.5, 2.5)     << "\n";
    std::cout << "square(3.14)    = " << square(3.14)       << "\n";

    std::vector<int> v = {1, 2, 3, 4, 5};
    std::cout << "getSize(vector) = " << getSize(v)         << "\n";

    std::string s = "hello";
    std::cout << "getSize(string) = " << getSize(s)         << "\n";

    // Try uncommenting these to see the nightmare error messages:
    // add("hello", "world");   // wall of template errors
    // square(42);              // another wall
    return 0;
}
