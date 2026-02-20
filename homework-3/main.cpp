#include <cassert>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

#include "fixed_pool_allocator.hpp"
#include "forward_list.hpp"

namespace {

int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

template <typename Map>
void fill_map(Map& map) {
    for (int i = 0; i < 10; ++i) {
        map.emplace(i, factorial(i));
    }
}

template <typename Map>
void print_map(const Map& map) {
    for (const auto& [key, value] : map) {
        std::cout << key << ' ' << value << '\n';
    }
}

template <typename Container>
void fill_container(Container& container) {
    for (int i = 0; i < 10; ++i) {
        container.push_back(i);
    }
}

template <typename Container>
void print_container(const Container& container) {
    for (const auto& value : container) {
        std::cout << value << '\n';
    }
}

}  // namespace

int main() {
    std::map<int, int> default_map;
    fill_map(default_map);
    assert(default_map.size() == 10);
    print_map(default_map);

    using MapValue = std::pair<const int, int>;
    using MapAllocator = FixedPoolAllocator<MapValue, 10>;
    std::map<int, int, std::less<int>, MapAllocator> map_with_custom_allocator;
    fill_map(map_with_custom_allocator);
    assert(map_with_custom_allocator.size() == 10);
    print_map(map_with_custom_allocator);

    ForwardList<int> default_container;
    fill_container(default_container);
    assert(default_container.size() == 10);
    print_container(default_container);

    using ContainerAllocator = FixedPoolAllocator<int, 10>;
    ForwardList<int, ContainerAllocator> custom_container;
    fill_container(custom_container);
    assert(custom_container.size() == 10);
    print_container(custom_container);

    return 0;
}
