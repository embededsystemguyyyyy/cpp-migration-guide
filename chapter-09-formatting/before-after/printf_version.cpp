// printf_version.cpp — Logging the old way: printf, no type safety, no love
// Compile: g++ -std=c++17 -o printf_version printf_version.cpp

#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

enum class LogLevel { DEBUG, INFO, WARN, ERROR };

const char* levelName(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
    }
    return "?????";
}

// No type safety — pass the wrong format specifier, get undefined behavior
void log(LogLevel level, const char* fmt, ...) {
    time_t now = time(nullptr);
    char   timebuf[20];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", localtime(&now));

    fprintf(stdout, "[%s] [%s] ", timebuf, levelName(level));

    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    fprintf(stdout, "\n");
}

struct Request {
    int         id;
    std::string method;
    std::string path;
    int         status;
    double      duration_ms;
};

int main() {
    log(LogLevel::INFO,  "Server starting on port %d", 8080);
    log(LogLevel::DEBUG, "Config loaded from %s", "/etc/app/config.json");

    std::vector<Request> requests = {
        {1, "GET",  "/api/users",    200, 12.4},
        {2, "POST", "/api/login",    401, 3.1 },
        {3, "GET",  "/api/products", 200, 45.7},
    };

    for (const auto& r : requests) {
        // Type mismatch here would be a runtime disaster, not a compile error
        log(LogLevel::INFO, "Request #%d: %s %s -> %d (%.1fms)",
            r.id, r.method.c_str(), r.path.c_str(), r.status, r.duration_ms);
    }

    log(LogLevel::WARN,  "Memory usage at %d%%", 87);
    log(LogLevel::ERROR, "Database connection failed: %s", "timeout after 30s");

    return 0;
}
