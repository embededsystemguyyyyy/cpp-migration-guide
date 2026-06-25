// raw_array_version.cpp — 2D matrix using raw pointers: fast but dangerous
// Compile: g++ -std=c++17 -o raw_array_version raw_array_version.cpp

#include <iostream>
#include <stdexcept>

// Manual 2D matrix — one wrong index and you're reading garbage memory
class Matrix {
    double* data;
    int     rows, cols;

public:
    Matrix(int r, int c) : rows(r), cols(c) {
        data = new double[r * c]();  // zero-initialized
    }

    ~Matrix() { delete[] data; }

    // No copy constructor = double-free if you copy this matrix. Oops.
    Matrix(const Matrix&) = delete;
    Matrix& operator=(const Matrix&) = delete;

    // Manual index arithmetic — easy to get wrong
    double& at(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols)
            throw std::out_of_range("Index out of bounds");
        return data[r * cols + c];
    }

    const double& at(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols)
            throw std::out_of_range("Index out of bounds");
        return data[r * cols + c];
    }

    // Matrix multiply — raw pointer arithmetic, easy to introduce subtle bugs
    Matrix multiply(const Matrix& other) const {
        if (cols != other.rows)
            throw std::invalid_argument("Incompatible matrix dimensions");
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < other.cols; ++j)
                for (int k = 0; k < cols; ++k)
                    result.data[i * other.cols + j] +=
                        data[i * cols + k] * other.data[k * other.cols + j];
        return result;
    }

    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                std::cout << data[i * cols + j] << "\t";
            std::cout << "\n";
        }
    }
};

int main() {
    Matrix a(2, 3), b(3, 2);

    a.at(0,0)=1; a.at(0,1)=2; a.at(0,2)=3;
    a.at(1,0)=4; a.at(1,1)=5; a.at(1,2)=6;

    b.at(0,0)=7;  b.at(0,1)=8;
    b.at(1,0)=9;  b.at(1,1)=10;
    b.at(2,0)=11; b.at(2,1)=12;

    std::cout << "Matrix A:\n"; a.print();
    std::cout << "Matrix B:\n"; b.print();

    auto c = a.multiply(b);
    std::cout << "A * B:\n"; c.print();

    return 0;
}
