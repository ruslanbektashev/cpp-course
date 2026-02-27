#include "matrix.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Тест 1: " << std::endl;
    
    Matrix<int, -1> matrix; 
    assert(matrix.size() == 0); 
    std::cout << "Начальный размер: " << matrix.size() << " (ожидаемое значение: 0 )\n";

    auto a = matrix[0][0];
    assert(a == -1);
    assert(matrix.size() == 0);
    std::cout << "Чтение из [0][0]: " << a << " (ожидаемое значение: -1)\n";
    std::cout << "Размер после чтения: " << matrix.size() << " (ожидаемое значение: 0)\n";

    matrix[100][100] = 314;
    assert(matrix[100][100] == 314);
    assert(matrix.size() == 1);
    std::cout << "matrix[100][100] = 314\n";
    std::cout << "Размер после присвоения: " << matrix.size() << " (ожидаемое значение: 1)\n";

    std::cout << "\nИтерация по занятым ячейкам:\n";
    for (auto c : matrix) {
        int i, j, v;
        std::tie(i, j, v) = c;
        std::cout << i << j << v << std::endl;
    }

    std::cout << "\nТест 2: \n";
    
    Matrix<int, 0> testMatrix;
    std::cout << "\n1. Заполнение главной диагонали (от [0,0] до [9,9]) значениями от 0 до 9\n";
    for (int i = 0; i < 10; ++i) {
        testMatrix[i][i] = i;
    }
    
    std::cout << "2. Заполнение второстепенной диагонали (от [0,9] до [9,0]) значениями от 9 до 0\n";
    for (int i = 0; i < 10; ++i) {
        testMatrix[i][9 - i] = 9 - i;
    }
    
    std::cout << "\n3. Фрагмент матрицы от [1,1] до [8,8]:" << std::endl;
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= 8; ++j) {
            std::cout << testMatrix[i][j];
            if (j < 8) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n4. Количество занятых ячеек: " << testMatrix.size() << std::endl;
    
    std::cout << "\n5. Все занятые ячейки вместе со своими позициями:" << std::endl;
    for (auto cell : testMatrix) {
        int row, col, value;
        std::tie(row, col, value) = cell;
        std::cout << "[" << row << "][" << col << "] = " << value << std::endl;
    }
    
    std::cout << "\n=== Все тесты успешно завершены ===" << std::endl;
    
    return 0;
}