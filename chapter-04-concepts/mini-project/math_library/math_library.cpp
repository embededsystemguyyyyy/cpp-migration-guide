// math_library.cpp — Type-safe math library using C++20 Concepts
// Compile: g++ -std=c++20 -o math_library math_library.cpp

#include <concepts>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

// ── Concepts ──────────────────────────────────────────────────────────────

// Any numeric type (int, float, double, etc.)
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Only floating point types (for operations that need precision)
template <typename T>
concept Real = std::floating_point<T>;

// A type that can be used as a 2D vector component
template <typename T>
concept VectorComponent = Numeric<T> && std::is_copy_constructible_v<T>;

// ── Basic Operations ───────────────────────────────────────────────────────

template <Numeric T>
T clamp(T value, T min, T max) {
    if (min > max) throw std::invalid_argument("min must be <= max");
    return value < min ? min : (value > max ? max : value);
}

template <Numeric T>
T lerp(T a, T b, double t) {
    return static_cast<T>(a + t * (b - a));
}

template <Real T>
T toRadians(T degrees) {
    constexpr T PI = static_cast<T>(3.14159265358979323846);
    return degrees * (PI / static_cast<T>(180));
}

template <Real T>
T toDegrees(T radians) {
    constexpr T PI = static_cast<T>(3.14159265358979323846);
    return radians * (static_cast<T>(180) / PI);
}

// ── 2D Vector ─────────────────────────────────────────────────────────────

template <VectorComponent T>
struct Vec2 {
    T x, y;

    Vec2(T x, T y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(T scalar)          const { return {x * scalar,  y * scalar};  }

    T dot(const Vec2& other) const { return x * other.x + y * other.y; }

    // Only available for floating point vectors
    double length() const requires Real<T> {
        return std::sqrt(static_cast<double>(x * x + y * y));
    }

    Vec2 normalized() const requires Real<T> {
        double len = length();
        if (len == 0) throw std::runtime_error("Cannot normalize zero vector");
        return { static_cast<T>(x / len), static_cast<T>(y / len) };
    }

    std::string toString() const {
        return "Vec2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// ── Main ──────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== Type-Safe Math Library ===\n\n";

    // clamp
    std::cout << "clamp(15, 0, 10)    = " << clamp(15, 0, 10)       << "\n";
    std::cout << "clamp(5, 0, 10)     = " << clamp(5, 0, 10)        << "\n";
    std::cout << "clamp(-3, 0, 10)    = " << clamp(-3, 0, 10)       << "\n\n";

    // lerp
    std::cout << "lerp(0, 100, 0.25)  = " << lerp(0, 100, 0.25)     << "\n";
    std::cout << "lerp(0.0, 1.0, 0.5) = " << lerp(0.0, 1.0, 0.5)   << "\n\n";

    // angle conversion
    std::cout << "toRadians(180.0)    = " << toRadians(180.0)        << "\n";
    std::cout << "toDegrees(3.14159)  = " << toDegrees(3.14159)      << "\n\n";

    // Vec2 with floats
    Vec2<float> a{3.0f, 4.0f};
    Vec2<float> b{1.0f, 2.0f};
    std::cout << "a                   = " << a.toString()             << "\n";
    std::cout << "b                   = " << b.toString()             << "\n";
    std::cout << "a + b               = " << (a + b).toString()       << "\n";
    std::cout << "a.dot(b)            = " << a.dot(b)                 << "\n";
    std::cout << "a.length()          = " << a.length()               << "\n";
    std::cout << "a.normalized()      = " << a.normalized().toString() << "\n\n";

    // Vec2 with ints — length() and normalized() not available (compile-time!)
    Vec2<int> c{3, 4};
    Vec2<int> d{1, 2};
    std::cout << "c + d               = " << (c + d).toString()       << "\n";
    std::cout << "c.dot(d)            = " << c.dot(d)                 << "\n";

    // Uncommenting this line gives a clear compile error:
    // c.length();
    // error: 'length' requires 'Real<int>' which is not satisfied

    return 0;
}
