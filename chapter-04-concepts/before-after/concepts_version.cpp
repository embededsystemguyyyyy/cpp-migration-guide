// concepts_version.cpp — The clean modern way
// Same constraints as sfinae_version.cpp — but readable and friendly
// Compile: g++ -std=c++20 -o concepts_version concepts_version.cpp

#include <concepts>
#include <iostream>
#include <string>
#include <vector>

// Concept: reads like English, enforced at the call site
template <std::arithmetic T>
T add(T a, T b) {
    return a + b;
}

// Custom concept: any type that has a .size() method returning a size_type
template <typename T>
concept Sizeable = requires(T c) {
    { c.size() } -> std::convertible_to<std::size_t>;
};

template <Sizeable Container>
auto getSize(const Container& c) {
    return c.size();
}

// Constraint directly in the requires clause — intent is crystal clear
template <typename T>
requires std::floating_point<T>
T square(T x) {
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

    // Try uncommenting these — error messages are now human readable:
    // add("hello", "world");
    // Error: 'const char*' does not satisfy constraint 'arithmetic'
    //
    // square(42);
    // Error: 'int' does not satisfy constraint 'floating_point'
    return 0;
}
