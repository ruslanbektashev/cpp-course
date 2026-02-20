#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

struct Config {
    int rows = 100;
    int cols = 100;
    int density_percent = 5;
    std::uint32_t seed = 42;
    bool inject_cycle = false;
};

Config parse_args(int argc, char* argv[]) {
    Config cfg;
    if (argc >= 2) {
        cfg.rows = std::stoi(argv[1]);
    }
    if (argc >= 3) {
        cfg.cols = std::stoi(argv[2]);
    }
    if (argc >= 4) {
        cfg.density_percent = std::stoi(argv[3]);
    }
    if (argc >= 5) {
        cfg.seed = static_cast<std::uint32_t>(std::stoul(argv[4]));
    }
    if (argc >= 6) {
        cfg.inject_cycle = std::stoi(argv[5]) != 0;
    }
    return cfg;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const Config cfg = parse_args(argc, argv);

        if (cfg.rows <= 0 || cfg.cols <= 0 || cfg.density_percent < 0 || cfg.density_percent > 100) {
            std::cerr << "Usage: " << argv[0]
                      << " [rows>0] [cols>0] [density:0..100] [seed] [inject_cycle:0|1]" << '\n';
            return 1;
        }

        std::mt19937 rng(cfg.seed);
        std::bernoulli_distribution bit(static_cast<double>(cfg.density_percent) / 100.0);

        std::vector<std::string> matrix(static_cast<std::size_t>(cfg.rows),
                                        std::string(static_cast<std::size_t>(cfg.cols), '0'));

        for (int r = 0; r < cfg.rows; ++r) {
            for (int c = 0; c < cfg.cols; ++c) {
                matrix[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] = bit(rng) ? '1' : '0';
            }
        }

        if (cfg.inject_cycle && cfg.rows >= 2 && cfg.cols >= 2) {
            std::uniform_int_distribution<int> row_dist(0, cfg.rows - 1);
            std::uniform_int_distribution<int> col_dist(0, cfg.cols - 1);

            int r1 = row_dist(rng);
            int r2 = row_dist(rng);
            while (r2 == r1) {
                r2 = row_dist(rng);
            }

            int c1 = col_dist(rng);
            int c2 = col_dist(rng);
            while (c2 == c1) {
                c2 = col_dist(rng);
            }

            matrix[static_cast<std::size_t>(r1)][static_cast<std::size_t>(c1)] = '1';
            matrix[static_cast<std::size_t>(r1)][static_cast<std::size_t>(c2)] = '1';
            matrix[static_cast<std::size_t>(r2)][static_cast<std::size_t>(c1)] = '1';
            matrix[static_cast<std::size_t>(r2)][static_cast<std::size_t>(c2)] = '1';
        }

        std::cout << cfg.rows << ' ' << cfg.cols << '\n';
        for (const auto& row : matrix) {
            std::cout << row << '\n';
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
