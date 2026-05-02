
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * Структура для хранения данных одной строки.
 * text1 - IPv4 адрес (проверяется)
 * text2, text3 - игнорируются
 * ipParts - хранит численные значения октетов IP для сортировки
 */
struct Record {
  std::string text1;
  std::string text2;
  std::string text3;
  std::vector<int> ipParts;
};

// Функция для парсинга и проверки IPv4
// Возвращает вектор октетов, если IP валиден, иначе пустой вектор
std::vector<int> parseIPv4(const std::string &ip) {
  std::stringstream ss(ip);
  std::string segment;
  std::vector<int> parts;

  while (std::getline(ss, segment, '.')) {
    try {
      int val = std::stoi(segment); 
      if (val >= 0 && val <= 255) {
        parts.push_back(val);
      }
    } catch (...) {
      // Игнорируем ошибки парсинга
    }
  }

  if (parts.size() == 4) { // если октетов 4
    return parts;
  } else {
    std::cerr << "Invalid IP format: " << ip << std::endl;
    return {};
  }
}

int main() {
  std::string line;
  std::vector<Record>
      records; 

  while (std::getline(std::cin, line)) {
    if (line.empty())
      continue;

    std::stringstream ss(line); // Создаем поток из строки для удобного чтения
    Record record;              // Временный объект для хранения текущей строки

    // Поля разделены символом табуляции '\t'
    if (std::getline(ss, record.text1, '\t') &&
        std::getline(ss, record.text2, '\t') &&
        std::getline(ss,
                     record.text3)) { // Остаток строки (text3) до конца или \t

      // Проверяем первый элемент (IPv4) и получаем его части
      std::vector<int> parts = parseIPv4(record.text1);

      // Если IP валиден, сохраняем запись
      if (!parts.empty()) {
        record.ipParts = parts;
        records.push_back(record);
      }

    } else {
      std::cerr << "Not formatted string " << line << std::endl;
    }
  }

  // Сортируем записи в обратном лексикографическом порядке
  // Используем stable_sort для сохранения относительного порядка одинаковых
  // элементов
  std::stable_sort(
      records.begin(), records.end(), [](const Record &a, const Record &b) {
        return a.ipParts >
               b.ipParts; // Сравниваем векторы чисел (лексикографически)
      });

  // 1. Полный список адресов после сортировки
  for (const auto &rec : records) {
    std::cout << rec.text1 << std::endl;
  }

  // 2. Список адресов, первый байт которых равен 1
  for (const auto &rec : records) {
    if (rec.ipParts[0] == 1) {
      std::cout << rec.text1 << std::endl;
    }
  }

  // 3. Список адресов, первый байт 46, второй 70
  for (const auto &rec : records) {
    if (rec.ipParts[0] == 46 && rec.ipParts[1] == 70) {
      std::cout << rec.text1 << std::endl;
    }
  }

  // 4. Список адресов, любой байт которых равен 46
  for (const auto &rec : records) {
    bool has46 = false;
    for (int part : rec.ipParts) {
      if (part == 46) {
        has46 = true;
        break;
      }
    }
    if (has46) {
      std::cout << rec.text1 << std::endl;
    }
  }

  return 0;
}
