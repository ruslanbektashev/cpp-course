#include "matrix.h"
#include "print_ip.h"

#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <vector>

int main()
{
  // --- IP printer demo/tests (from task statement) ---
  print_ip(std::int8_t{-1});
  std::cout << '\n';

  print_ip(std::int16_t{0});
  std::cout << '\n';

  print_ip(std::int32_t{2130706433});
  std::cout << '\n';

  print_ip(std::int64_t{8875824491850138409LL});
  std::cout << '\n';

  print_ip(std::string{"Hello, World!"});
  std::cout << '\n';

  print_ip(std::vector<int>{100, 200, 300, 400});
  std::cout << '\n';

  print_ip(std::list<short>{400, 300, 200, 100});
  std::cout << '\n';

  // --- Infinite matrix demo (classic OTUS-style scenario) ---
  Matrix<int, 0> matrix;

  for (std::size_t i = 0; i < 10; ++i) {
    matrix[i][i] = static_cast<int>(i);
    matrix[i][9 - i] = static_cast<int>(9 - i);
  }

  // Print 10x10 (unset cells are default=0)
  for (std::size_t row = 0; row < 10; ++row) {
    for (std::size_t col = 0; col < 10; ++col) {
      if (col) {
        std::cout << ' ';
      }
      std::cout << static_cast<int>(matrix[row][col]);
    }
    std::cout << '\n';
  }

  std::cout << "stored=" << matrix.size() << '\n';

  for (const auto& [row, col, value] : matrix) {
    std::cout << row << ' ' << col << ' ' << value << '\n';
  }

  return 0;
}
