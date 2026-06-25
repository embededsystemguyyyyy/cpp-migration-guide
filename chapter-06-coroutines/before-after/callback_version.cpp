// callback_version.cpp — Async via callbacks: it works, but good luck maintaining it
// Compile: g++ -std=c++17 -pthread -o callback_version callback_version.cpp

#include <functional>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

// Simulated async operations using raw callbacks
// Notice: error handling is an afterthought, nesting gets deep fast

using Callback      = std::function<void(const std::string&)>;
using ErrorCallback = std::function<void(const std::string&)>;

void fetchUserAsync(int userId,
                    Callback onSuccess,
                    ErrorCallback onError) {
    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (userId <= 0) {
            onError("Invalid user ID");
            return;
        }
        onSuccess("User_" + std::to_string(userId));
    }).detach();
}

void fetchOrdersAsync(const std::string& username,
                      Callback onSuccess,
                      ErrorCallback onError) {
    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (username.empty()) {
            onError("Empty username");
            return;
        }
        onSuccess("Orders_for_" + username);
    }).detach();
}

void processOrderAsync(const std::string& orders,
                       Callback onSuccess,
                       ErrorCallback onError) {
    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        onSuccess("Processed: " + orders);
    }).detach();
}

int main() {
    std::promise<void> done;
    auto future = done.get_future();

    // Callback pyramid of doom — and we only have 3 levels here
    fetchUserAsync(42,
        [&](const std::string& user) {
            fetchOrdersAsync(user,
                [&](const std::string& orders) {
                    processOrderAsync(orders,
                        [&](const std::string& result) {
                            std::cout << "Final result: " << result << "\n";
                            done.set_value(); // signal completion
                        },
                        [&](const std::string& err) {
                            std::cerr << "Process error: " << err << "\n";
                            done.set_value();
                        });
                },
                [&](const std::string& err) {
                    std::cerr << "Orders error: " << err << "\n";
                    done.set_value();
                });
        },
        [&](const std::string& err) {
            std::cerr << "User error: " << err << "\n";
            done.set_value();
        });

    future.wait();
    return 0;
}
