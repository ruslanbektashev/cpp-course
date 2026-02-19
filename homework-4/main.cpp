#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using Ip = std::array<int, 4>;

static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::string item;
    std::stringstream ss(s);
    while (std::getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

static Ip parse_ip(const std::string& s) {
    auto parts = split(s, '.');
    if (parts.size() != 4) throw std::runtime_error("Invalid IP: " + s);

    Ip ip{};
    for (size_t i = 0; i < 4; ++i) {
        int v = std::stoi(parts[i]);
        if (v < 0 || v > 255) throw std::runtime_error("Invalid IP byte: " + s);
        ip[i] = v;
    }
    return ip;
}

static void print_ip(const Ip& ip) {
    std::cout << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";
}

int main() {
    try {
        std::vector<Ip> pool;
        std::string line;

        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;

            // text1 \t text2 \t text3
            auto tab_pos = line.find('\t');
            std::string ip_str = (tab_pos == std::string::npos) ? line : line.substr(0, tab_pos);

            pool.push_back(parse_ip(ip_str));
        }

        // reverse lexicographic by bytes (numeric)
        std::sort(pool.begin(), pool.end(), [](const Ip& a, const Ip& b) {
            if (a[0] != b[0]) return a[0] > b[0];
            if (a[1] != b[1]) return a[1] > b[1];
            if (a[2] != b[2]) return a[2] > b[2];
            return a[3] > b[3];
        });

        // 1) full list
        for (const auto& ip : pool) print_ip(ip);

        // 2) first byte == 1
        for (const auto& ip : pool)
            if (ip[0] == 1) print_ip(ip);

        // 3) first == 46 and second == 70
        for (const auto& ip : pool)
            if (ip[0] == 46 && ip[1] == 70) print_ip(ip);

        // 4) any byte == 46
        for (const auto& ip : pool)
            if (ip[0] == 46 || ip[1] == 46 || ip[2] == 46 || ip[3] == 46)
                print_ip(ip);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
