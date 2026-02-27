#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <sstream>

using IPAddress = std::tuple<int, int, int, int>;

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

IPAddress parseIP(const std::string& ip_str) {
    auto octets = split(ip_str, '.');
    
    return std::make_tuple(
        std::stoi(octets[0]),
        std::stoi(octets[1]),
        std::stoi(octets[2]),
        std::stoi(octets[3])
    );
}

void printIP(const IPAddress& ip) {
    std::cout << std::get<0>(ip) << "." 
              << std::get<1>(ip) << "."
              << std::get<2>(ip) << "."
              << std::get<3>(ip) << "\n";
}

void printAll(const std::vector<IPAddress>& pool) {
    for (const auto& ip : pool) {
        printIP(ip);
    }
}

void filterByFirst(const std::vector<IPAddress>& pool, int first) {
    for (const auto& ip : pool) {
        if (std::get<0>(ip) == first) {
            printIP(ip);
        }
    }
}

void filterByFirstTwo(const std::vector<IPAddress>& pool, int first, int second) {
    for (const auto& ip : pool) {
        if (std::get<0>(ip) == first && std::get<1>(ip) == second) {
            printIP(ip);
        }
    }
}

void filterByAny(const std::vector<IPAddress>& pool, int value) {
    for (const auto& ip : pool) {
        if (std::get<0>(ip) == value || 
            std::get<1>(ip) == value || 
            std::get<2>(ip) == value || 
            std::get<3>(ip) == value) {
            printIP(ip);
        }
    }
}

int main() {
    try {
        std::vector<IPAddress> ip_pool;
        
        std::string line;
        while (std::getline(std::cin, line)) {
            auto parts = split(line, '\t');
            
            if (!parts.empty()) {
                IPAddress ip = parseIP(parts[0]);
                ip_pool.push_back(ip);
            }
        }
        
        std::sort(ip_pool.begin(), ip_pool.end(), std::greater<IPAddress>());
        
        printAll(ip_pool);
        
        filterByFirst(ip_pool, 1);
        
        filterByFirstTwo(ip_pool, 46, 70);
        
        filterByAny(ip_pool, 46);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}