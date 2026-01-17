#include "sparse_matrix.hpp"
#include <cassert>

int main()
{
    // SparseMatrixNew<float> matrix(0);


    // matrix.showMatrix();
    // std::cout << "getValue(1, 2): " << matrix.getValue({0, 0}) << std::endl;
    // std::cout << "matrix[1][2]: " << (*matrix.begin()).second << std::endl;
    // std::cout << "Non-default count: " << matrix.countNonDefault() << std::endl;

    SparseMatrixNew<double> matrix(-1);
    assert(matrix.size() == 0); // все ячейки свободны

    auto a = matrix[0][0];
    assert(a == -1);
    assert(matrix.size() == 0);

    matrix[100][100] = 314;
    assert(matrix[100][100] == 314);
    assert(matrix.size() == 1);

    (matrix[1][0] = 5.0) = 4.1;
    matrix[0][0] = 3.0;
    matrix[1][2] = 12.0;
    // выведется одна строка
    // 100100314
    // for (const auto &c : matrix)
    // {
    //     int i, j, v;
    //     std::tie(i, j, v) = c;
    //     std::cout << i << j << v << std::endl;
    // }
    matrix.showMatrix();
    std::cout << "matrix begin: " << matrix.begin()->second << std::endl;
    std::cout << "matrix end: " << std::prev(matrix.end())->second << std::endl;
    for (const auto &element : matrix)
    {
        std::cout << element.second << " ";
        for (const auto& coord : element.first )
        {
            std::cout << coord << " ";
        }
        std::cout << "\n";
    }
    return 0;
}