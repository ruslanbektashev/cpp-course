#include "sparse_matrix.hpp"


int main()
{
    SparseMatrixNew<float> matrix(0);
    matrix[1][0] = 5.0;
    matrix[0][0] = 3.0;
    matrix[1][2] = 12.0;

    matrix.showMatrix();
    std::cout << "getValue(1, 2): " << matrix.getValue({1, 2}) << std::endl;
    std::cout << "matrix[1][2]: " << matrix[1][2] << std::endl;
    std::cout << "Non-default count: " << matrix.countNonDefault() << std::endl;

    return 0;
}