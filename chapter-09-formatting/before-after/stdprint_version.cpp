// stdprint_version.cpp — Same logger as printf_version.cpp using C++23 std::print
// Type safe. Readable. No more .c_str() everywhere.
// Compile: g++ -std=c++23 -o stdprint_version stdprint_version.cpp

#include <chrono>
#include <format>
#include <print>
#include <string>
#include <vector>

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

std::string_view levelName(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
    }
    return "?????";
}

// Type safe — wrong argument type is a compile error, not undefined behavior
template <typename... Args>
void log(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::zoned_time{std::chrono::current_zone(), now};

    std::println("[{:%H:%M:%S}] [{}] {}",
                 time,
                 levelName(level),
                 std::format(fmt, std::forward<Args>(args)...));
}

struct Request {
    int         id;
    std::string method;
    std::string path;
    int         status;
    double      duration_ms;
};

int main() {
    log(LogLevel::INFO,  "Server starting on port {}", 8080);
    log(LogLevel::DEBUG, "Config loaded from {}", "/etc/app/config.json");

    std::vector<Request> requests = {
        {1, "GET",  "/api/users",    200, 12.4},
        {2, "POST", "/api/login",    401, 3.1 },
        {3, "GET",  "/api/products", 200, 45.7},
    };

    for (const auto& r : requests) {
        // Passing a string directly — no .c_str() needed
        // Wrong type here? Compile error. Not a runtime crash.
        log(LogLevel::INFO, "Request #{}: {} {} -> {} ({:.1f}ms)",
            r.id, r.method, r.path, r.status, r.duration_ms);
    }

    log(LogLevel::WARN,  "Memory usage at {}%", 87);
    log(LogLevel::ERROR, "Database connection failed: {}", "timeout after 30s");

    return 0;
}
