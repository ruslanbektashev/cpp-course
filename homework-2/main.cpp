#include <cassert>
#include <iostream>
#include <tuple>

#include "matrix.hpp"

namespace {

void run_assignment_scenario() {
    Matrix<int, 0> matrix;

    for (int i = 0; i < 10; ++i) {
        matrix[i][i] = i;
    }

    for (int i = 0; i < 10; ++i) {
        matrix[i][9 - i] = 9 - i;
    }

    for (int row = 1; row <= 8; ++row) {
        for (int col = 1; col <= 8; ++col) {
            if (col > 1) {
                std::cout << ' ';
            }
            std::cout << matrix[row][col];
        }
        std::cout << '\n';
    }

    std::cout << "occupied: " << matrix.size() << '\n';

    for (const auto cell : matrix) {
        Matrix<int, 0>::index_type row{};
        Matrix<int, 0>::index_type col{};
        int value{};
        std::tie(row, col, value) = cell;
        std::cout << row << ' ' << col << ' ' << value << '\n';
    }
}

void run_readme_example() {
    Matrix<int, -1> matrix;
    assert(matrix.size() == 0);

    const int a = matrix[0][0];
    assert(a == -1);
    assert(matrix.size() == 0);

    matrix[100][100] = 314;
    assert(matrix[100][100] == 314);
    assert(matrix.size() == 1);

    ((matrix[100][100] = 314) = -1) = 217;
    assert(matrix[100][100] == 217);
    assert(matrix.size() == 1);

    for (const auto cell : matrix) {
        int row{};
        int col{};
        int value{};
        std::tie(row, col, value) = cell;
        std::cout << row << col << value << '\n';
    }
}

}  // namespace

int main() {
    run_assignment_scenario();
    run_readme_example();
    return 0;
}
