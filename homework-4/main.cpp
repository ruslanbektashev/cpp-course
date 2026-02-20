#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Ip = std::array<int, 4>;

constexpr int FILTER_FIRST_OCTET = 1;
constexpr int FILTER_FIRST_OCTET_2 = 46;
constexpr int FILTER_SECOND_OCTET_2 = 70;
constexpr int FILTER_ANY_OCTET = 46;

Ip parse_ip(const std::string& text) {
    Ip ip{0, 0, 0, 0};
    std::stringstream stream(text);
    std::string part;

    for (std::size_t i = 0; i < ip.size(); ++i) {
        if (!std::getline(stream, part, '.')) {
            throw std::runtime_error("Invalid IP format");
        }
        ip[i] = std::stoi(part);
    }

    return ip;
}

void print_ip(const Ip& ip) {
    std::cout << ip[0] << '.'
              << ip[1] << '.'
              << ip[2] << '.'
              << ip[3] << '\n';
}

template <typename Predicate>
void print_filtered(const std::vector<Ip>& ips, Predicate predicate) {
    for (const Ip& ip : ips) {
        if (predicate(ip)) {
            print_ip(ip);
        }
    }
}

bool filter_first(const Ip& ip, int value) {
    return ip[0] == value;
}

bool filter_first_second(const Ip& ip, int first, int second) {
    return ip[0] == first && ip[1] == second;
}

bool filter_any(const Ip& ip, int value) {
    return std::any_of(ip.begin(), ip.end(),
        [value](int octet) { return octet == value; });
}

} 

int main() {
    std::vector<Ip> ips;
    std::string line;

    std::cout << "Reading IP addresses from stdin (Ctrl+D to finish input)...\n";

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        const std::size_t tab_pos = line.find('\t');
        const std::string ip_text = line.substr(0, tab_pos);

        ips.push_back(parse_ip(ip_text));
    }

    std::sort(ips.begin(), ips.end(), std::greater<Ip>{});

    print_filtered(ips, [](const Ip&) { return true; });

    print_filtered(ips, [](const Ip& ip) {
        return filter_first(ip, FILTER_FIRST_OCTET);
    });

    print_filtered(ips, [](const Ip& ip) {
        return filter_first_second(ip,
                                   FILTER_FIRST_OCTET_2,
                                   FILTER_SECOND_OCTET_2);
    });

    print_filtered(ips, [](const Ip& ip) {
        return filter_any(ip, FILTER_ANY_OCTET);
    });

    return 0;
}