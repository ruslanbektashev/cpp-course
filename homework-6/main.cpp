#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool has_cycle_4(std::istream& input) {
    int rows = 0;
    int cols = 0;
    if (!(input >> rows >> cols)) {
        throw std::runtime_error("Failed to read matrix dimensions");
    }

    std::unordered_set<std::uint64_t> seen_pairs;
    std::string row;
    row.reserve(static_cast<std::size_t>(cols));

    for (int r = 0; r < rows; ++r) {
        if (!(input >> row)) {
            throw std::runtime_error("Failed to read matrix row");
        }
        if (static_cast<int>(row.size()) != cols) {
            throw std::runtime_error("Invalid row length");
        }

        std::vector<int> ones;
        ones.reserve(static_cast<std::size_t>(cols / 4));
        for (int c = 0; c < cols; ++c) {
            if (row[static_cast<std::size_t>(c)] == '1') {
                ones.push_back(c);
            }
        }

        for (std::size_t i = 0; i < ones.size(); ++i) {
            for (std::size_t j = i + 1; j < ones.size(); ++j) {
                const std::uint64_t key =
                    static_cast<std::uint64_t>(ones[i]) * static_cast<std::uint64_t>(cols) +
                    static_cast<std::uint64_t>(ones[j]);
                if (seen_pairs.find(key) != seen_pairs.end()) {
                    return true;
                }
                seen_pairs.insert(key);
            }
        }
    }

    return false;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc > 2) {
            std::cerr << "Usage: " << argv[0] << " [input_file]" << '\n';
            return 1;
        }

        if (argc == 2) {
            std::ifstream file(argv[1]);
            if (!file) {
                std::cerr << "Failed to open file: " << argv[1] << '\n';
                return 1;
            }
            std::cout << (has_cycle_4(file) ? 1 : 0) << '\n';
            return 0;
        }

        std::cout << (has_cycle_4(std::cin) ? 1 : 0) << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
