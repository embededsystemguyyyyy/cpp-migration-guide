// migration_audit.cpp
// Scans a directory of .cpp/.h files and flags legacy C++ patterns
// Compile: g++ -std=c++17 -o migration_audit migration_audit.cpp
// Usage:   ./migration_audit <path-to-source-dir>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Pattern {
    std::string name;
    std::regex  regex;
    std::string modern_replacement;
    int         priority; // 1=High 2=Medium 3=Low
};

struct Finding {
    std::string file;
    int         line;
    std::string pattern_name;
    std::string matched_text;
    std::string fix;
    int         priority;
};

// All legacy patterns we look for
const std::vector<Pattern> PATTERNS = {
    { "Raw owning pointer",
      std::regex(R"(\bnew\b)"),
      "Use std::unique_ptr or std::shared_ptr", 1 },

    { "Manual delete",
      std::regex(R"(\bdelete\b)"),
      "Use smart pointers — delete should be unnecessary", 1 },

    { "printf/fprintf/sprintf",
      std::regex(R"(\b(printf|fprintf|sprintf|scanf)\b)"),
      "Use std::print / std::println (C++23) or std::format", 2 },

    { "Explicit iterator in for loop",
      std::regex(R"(::iterator|::const_iterator)"),
      "Use range-based for loop or std::ranges algorithms", 2 },

    { "NULL macro",
      std::regex(R"(\bNULL\b)"),
      "Use nullptr", 3 },

    { "C-style cast",
      std::regex(R"(\(int\)|\(char\*\)|\(void\*\)|\(float\)|\(double\))"),
      "Use static_cast<>, reinterpret_cast<>, or const_cast<>", 2 },

    { "std::endl (flushes every line)",
      std::regex(R"(std::endl)"),
      "Use '\\n' — std::endl flushes the buffer every call (slow)", 3 },

    { "Functor struct with operator()",
      std::regex(R"(bool\s+operator\(\))"),
      "Replace with a lambda expression", 2 },
};

std::vector<Finding> auditFile(const fs::path& path) {
    std::vector<Finding> findings;
    std::ifstream file(path);
    if (!file.is_open()) return findings;

    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        ++lineNum;
        // Skip comment lines
        auto trimmed = line.find_first_not_of(" \t");
        if (trimmed != std::string::npos && line[trimmed] == '/') continue;

        for (const auto& pattern : PATTERNS) {
            std::smatch match;
            if (std::regex_search(line, match, pattern.regex)) {
                findings.push_back({
                    path.string(),
                    lineNum,
                    pattern.name,
                    match[0].str(),
                    pattern.modern_replacement,
                    pattern.priority
                });
            }
        }
    }
    return findings;
}

void printReport(const std::vector<Finding>& all) {
    // Group by priority
    std::map<int, std::vector<const Finding*>> byPriority;
    for (const auto& f : all) byPriority[f.priority].push_back(&f);

    const std::map<int, std::string> labels = {
        {1, "🔴 HIGH   "}, {2, "🟡 MEDIUM "}, {3, "🟢 LOW    "}
    };

    std::cout << "\n╔══════════════════════════════════════════════════╗\n";
    std::cout <<   "║        C++ LEGACY PATTERN AUDIT REPORT          ║\n";
    std::cout <<   "╚══════════════════════════════════════════════════╝\n\n";
    std::cout << "Total findings: " << all.size() << "\n\n";

    for (int p = 1; p <= 3; ++p) {
        if (byPriority.find(p) == byPriority.end()) continue;
        std::cout << labels.at(p) << "PRIORITY  ("
                  << byPriority[p].size() << " findings)\n";
        std::cout << std::string(50, '-') << "\n";

        for (const auto* f : byPriority[p]) {
            std::cout << "  Pattern : " << f->pattern_name << "\n";
            std::cout << "  File    : " << f->file
                      << " (line " << f->line << ")\n";
            std::cout << "  Found   : " << f->matched_text << "\n";
            std::cout << "  Fix     : " << f->fix << "\n\n";
        }
    }

    std::cout << "══════════════════════════════════════════════════\n";
    std::cout << "Migration readiness: ";
    if (all.empty()) {
        std::cout << "✅ Clean! No legacy patterns found.\n";
    } else {
        int high = byPriority.count(1) ? (int)byPriority[1].size() : 0;
        std::cout << (high == 0 ? "🟡 " : "🔴 ")
                  << high << " high-priority issues to fix before migrating.\n";
    }
}

int main(int argc, char* argv[]) {
    std::string searchPath = argc > 1 ? argv[1] : ".";

    if (!fs::exists(searchPath)) {
        std::cerr << "Path not found: " << searchPath << "\n";
        return 1;
    }

    std::cout << "Scanning: " << fs::absolute(searchPath) << "\n";

    std::vector<Finding> allFindings;
    for (const auto& entry : fs::recursive_directory_iterator(searchPath)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension().string();
        if (ext != ".cpp" && ext != ".h" && ext != ".hpp") continue;

        auto findings = auditFile(entry.path());
        allFindings.insert(allFindings.end(), findings.begin(), findings.end());
    }

    printReport(allFindings);
    return 0;
}
