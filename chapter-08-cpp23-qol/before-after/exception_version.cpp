// exception_version.cpp — Error handling via exceptions
// Compile: g++ -std=c++17 -o exception_version exception_version.cpp

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct User {
    int         id;
    std::string name;
    std::string email;
};

// Every function either returns a value or throws — the caller must guess which
double parseAge(const std::string& input) {
    try {
        double age = std::stod(input);
        if (age < 0 || age > 150)
            throw std::out_of_range("Age out of valid range: " + input);
        return age;
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Cannot parse age from: '" + input + "'");
    }
}

User findUser(int id, const std::vector<User>& users) {
    for (const auto& u : users) {
        if (u.id == id) return u;
    }
    throw std::runtime_error("User not found: " + std::to_string(id));
}

std::string validateEmail(const std::string& email) {
    if (email.find('@') == std::string::npos)
        throw std::invalid_argument("Invalid email: " + email);
    return email;
}

// Caller needs three separate catch blocks — and hopes nothing else throws
int main() {
    std::vector<User> users = {
        {1, "Alice", "alice@example.com"},
        {2, "Bob",   "bob_at_example"},    // bad email
    };

    // Finding a valid user
    try {
        User u = findUser(1, users);
        std::string email = validateEmail(u.email);
        std::cout << "Found: " << u.name << " <" << email << ">\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Validation error: " << e.what() << "\n";
    }

    // Missing user — exception thrown
    try {
        User u = findUser(99, users);
        std::cout << "Found: " << u.name << "\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
    }

    // Bad email — exception thrown
    try {
        User u = findUser(2, users);
        std::string email = validateEmail(u.email);
        std::cout << "Found: " << u.name << " <" << email << ">\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Validation error: " << e.what() << "\n";
    }

    // Age parsing
    try {
        double age = parseAge("not_a_number");
        std::cout << "Age: " << age << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
    }

    return 0;
}
