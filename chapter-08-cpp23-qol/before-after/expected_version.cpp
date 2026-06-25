// expected_version.cpp — Error handling via std::expected (C++23)
// Same logic as exception_version.cpp — errors are values, not surprises
// Compile: g++ -std=c++23 -o expected_version expected_version.cpp

#include <expected>
#include <iostream>
#include <print>
#include <string>
#include <vector>

struct User {
    int         id;
    std::string name;
    std::string email;
};

// Return type tells you everything: this either works or gives you an error string
std::expected<double, std::string> parseAge(const std::string& input) {
    try {
        double age = std::stod(input);
        if (age < 0 || age > 150)
            return std::unexpected("Age out of valid range: " + input);
        return age;
    } catch (...) {
        return std::unexpected("Cannot parse age from: '" + input + "'");
    }
}

std::expected<User, std::string> findUser(int id, const std::vector<User>& users) {
    for (const auto& u : users) {
        if (u.id == id) return u;
    }
    return std::unexpected("User not found: " + std::to_string(id));
}

std::expected<std::string, std::string> validateEmail(const std::string& email) {
    if (email.find('@') == std::string::npos)
        return std::unexpected("Invalid email: " + email);
    return email;
}

int main() {
    std::vector<User> users = {
        {1, "Alice", "alice@example.com"},
        {2, "Bob",   "bob_at_example"},
    };

    // Chain operations with and_then — only proceeds if each step succeeds
    auto result1 = findUser(1, users)
        .and_then([](const User& u) { return validateEmail(u.email)
            .transform([&](const std::string& email) {
                return u.name + " <" + email + ">";
            });
        });

    if (result1) std::println("Found: {}", *result1);
    else         std::println("Error: {}", result1.error());

    // Missing user — no exception, just an error value
    auto result2 = findUser(99, users);
    if (result2) std::println("Found: {}", result2->name);
    else         std::println("Error: {}", result2.error());

    // Bad email — error propagates cleanly
    auto result3 = findUser(2, users)
        .and_then([](const User& u) { return validateEmail(u.email); });

    if (result3) std::println("Email: {}", *result3);
    else         std::println("Error: {}", result3.error());

    // Age parsing
    auto age = parseAge("not_a_number");
    if (age)  std::println("Age: {}", *age);
    else      std::println("Parse error: {}", age.error());

    return 0;
}
