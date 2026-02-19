#include <iostream>
#include <tuple>
#include <cassert>
#include "matrix.hpp"

int main() {
    Matrix<int, 0> matrix; 

    // 1) Заполнить главную диагональ [0,0]..[9,9] значениями 0..9
    
    for (int i = 0; i <= 9; ++i) {
        matrix[i][i] = i;
    }

    // 2) Заполнить второстепенную диагональ [0,9]..[9,0] значениями 9..0
    for (int i = 0; i <= 9; ++i) {
        matrix[i][9 - i] = 9 - i;
    }

    // 3) Вывести фрагмент от [1,1] до [8,8], пробел между столбцами
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= 8; ++j) {
            std::cout << (int)matrix[i][j];
            if (j != 8) std::cout << ' ';
        }
        std::cout << '\n';
    }

    // 4) Вывести количество занятых ячеек
    std::cout << "occupied = " << matrix.size() << "\n";

    // 5) Вывести все занятые ячейки вместе с позициями
    for (auto cell : matrix) {
        int i, j, v;
        std::tie(i, j, v) = cell;
        std::cout << "[" << i << "," << j << "] = " << v << "\n";
    }

    
    Matrix<int, -1> m2;
    assert(m2.size() == 0);
    int a = m2[0][0];
    assert(a == -1);
    assert(m2.size() == 0);
    m2[100][100] = 314;
    assert((int)m2[100][100] == 314);
    assert(m2.size() == 1);

   
 
    ((m2[100][100] = 314) = -1) = 217;  
    assert((int)m2[100][100] == 217);
    assert(m2.size() == 1);

 
    m2[100][100] = -1;
    assert((int)m2[100][100] == -1);
    assert(m2.size() == 0);

    return 0;
}
