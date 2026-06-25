// coroutine_version.cpp — Same logic as callback_version.cpp using C++20 coroutines
// Reads like synchronous code. Runs asynchronously.
// Compile: g++ -std=c++20 -pthread -o coroutine_version coroutine_version.cpp

#include <coroutine>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

// ── Minimal Task<T> coroutine type ────────────────────────────────────────

template <typename T>
struct Task {
    struct promise_type {
        T value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never  initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T v) { value = std::move(v); }
        void unhandled_exception() { exception = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }

    T get() {
        // Spin until done (simplified — production code uses an executor)
        while (!handle.done()) std::this_thread::yield();
        if (handle.promise().exception)
            std::rethrow_exception(handle.promise().exception);
        return handle.promise().value;
    }
};

// ── Awaitable wrapper for thread-based async work ─────────────────────────

template <typename T>
struct AsyncOp {
    std::function<T()> work;

    bool await_ready() { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        std::thread([this, handle]() mutable {
            result = work();
            handle.resume();
        }).detach();
    }

    T await_resume() { return result; }
    T result;
};

// ── Simulated async operations — now plain functions ─────────────────────

AsyncOp<std::string> fetchUser(int userId) {
    return { [userId]() -> std::string {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (userId <= 0) throw std::runtime_error("Invalid user ID");
        return "User_" + std::to_string(userId);
    }};
}

AsyncOp<std::string> fetchOrders(const std::string& username) {
    return { [username]() -> std::string {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (username.empty()) throw std::runtime_error("Empty username");
        return "Orders_for_" + username;
    }};
}

AsyncOp<std::string> processOrder(const std::string& orders) {
    return { [orders]() -> std::string {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return "Processed: " + orders;
    }};
}

// ── The coroutine — reads exactly like synchronous code ───────────────────

Task<std::string> runPipeline() {
    // No nesting. No callbacks. Each step waits for the previous.
    auto user   = co_await fetchUser(42);
    auto orders = co_await fetchOrders(user);
    auto result = co_await processOrder(orders);
    co_return result;
}

int main() {
    auto task   = runPipeline();
    auto result = task.get();
    std::cout << "Final result: " << result << "\n";
    return 0;
}
