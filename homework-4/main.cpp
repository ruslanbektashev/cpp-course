
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

class IpAdress {
private:
    std::vector<int> octets;

public:
    IpAdress(std::string address) {
        std::stringstream ss(address);
        std::string segment;
        while (std::getline(ss, segment, '.')) {
            if (!segment.empty()) {
                octets.push_back(std::stoi(segment));
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const IpAdress& addr) {
        for (size_t i = 0; i < addr.octets.size(); ++i) {
            os << addr.octets[i] << (i == addr.octets.size() - 1 ? "" : ".");
        }
        return os;
    }

    bool operator<(const IpAdress& other) const { return octets < other.octets; }
    bool operator>(const IpAdress& other) const { return octets > other.octets; }
    
    const std::vector<int>& getOctets() const { return octets; }
};


int main() {
    std::vector<IpAdress> ipList;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string ip_str;
        if (std::getline(ss, ip_str, '\t')) {
            ipList.emplace_back(ip_str);
        }
    }

    for (size_t i = 0; i < ipList.size(); ++i) {
        for (size_t j = 0; j < ipList.size() - 1 - i; ++j) {
            if (ipList[j] < ipList[j + 1]) {
                IpAdress temp = ipList[j];
                ipList[j] = ipList[j + 1];
                ipList[j + 1] = temp;
            }
        }
    }


    for (const auto& item : ipList) {
        std::cout << item << std::endl;
    }
    
    for (const auto& item : ipList) {
        if (!item.getOctets().empty() && item.getOctets()[0] == 1) {
            std::cout << item << std::endl;
        }
    }

    for (const auto& item : ipList) {
        if (item.getOctets().size() >= 2 && item.getOctets()[0] == 46 && item.getOctets()[1] == 70) {
            std::cout << item << std::endl;
        }
    }

    for (const auto& item : ipList) {
        for (const auto& octet : item.getOctets()) {
            if (octet == 46) {
                std::cout << item << std::endl;
                break;
            }
        }
    }

    return 0;
}



