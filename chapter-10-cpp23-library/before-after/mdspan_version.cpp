// mdspan_version.cpp — Same matrix logic using C++23 std::mdspan
// No raw pointer arithmetic. No manual index math. No leaks.
// Compile: g++ -std=c++23 -o mdspan_version mdspan_version.cpp

#include <mdspan>
#include <print>
#include <stdexcept>
#include <vector>

using Matrix = std::mdspan<double, std::dextents<std::size_t, 2>>;

// Multiply two matrices — index access reads like math notation
std::vector<double> multiply(Matrix a, Matrix b) {
    if (a.extent(1) != b.extent(0))
        throw std::invalid_argument("Incompatible matrix dimensions");

    std::size_t rows = a.extent(0);
    std::size_t cols = b.extent(1);
    std::size_t inner = a.extent(1);

    std::vector<double> result_data(rows * cols, 0.0);
    Matrix result{result_data.data(), rows, cols};

    for (std::size_t i = 0; i < rows; ++i)
        for (std::size_t j = 0; j < cols; ++j)
            for (std::size_t k = 0; k < inner; ++k)
                result[i, j] += a[i, k] * b[k, j]; // C++23 multi-index operator

    return result_data;
}

void print(Matrix m, std::string_view label) {
    std::println("{}:", label);
    for (std::size_t i = 0; i < m.extent(0); ++i) {
        for (std::size_t j = 0; j < m.extent(1); ++j)
            std::print("{}\t", m[i, j]);
        std::println("");
    }
}

int main() {
    // Data lives in regular vectors — no manual new/delete
    std::vector<double> a_data = {1, 2, 3, 4, 5, 6};
    std::vector<double> b_data = {7, 8, 9, 10, 11, 12};

    Matrix a{a_data.data(), 2, 3};
    Matrix b{b_data.data(), 3, 2};

    print(a, "Matrix A");
    print(b, "Matrix B");

    auto   c_data = multiply(a, b);
    Matrix c{c_data.data(), 2, 2};
    print(c, "A * B");

    return 0;
}
