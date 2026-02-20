#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <type_traits>
#include <algorithm>
#include <tuple>

template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
print_ip(T val) {
    auto size = sizeof(T);
    auto* ptr = reinterpret_cast<uint8_t*>(&val);
    for (size_t i = 0; i < size; ++i) {
        std::cout << static_cast<int>(ptr[size - 1 - i]) << (i == size - 1 ? "" : ".");
    }
    std::cout << std::endl;
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value>::type
print_ip(T val) {
    std::cout << val << std::endl;
}

template <typename T>
typename std::enable_if<
    (std::is_same<T, std::vector<typename T::value_type>>::value || 
     std::is_same<T, std::list<typename T::value_type>>::value)
>::type
print_ip(const T& container) {
    for (auto it = container.begin(); it != container.end(); ++it) {
        if (it != container.begin()) std::cout << ".";
        std::cout << *it;
    }
    std::cout << std::endl;
}


template <size_t I = 0, typename... Args>
typename std::enable_if<I == sizeof...(Args)>::type
print_tuple(const std::tuple<Args...>&) {}

template <size_t I = 0, typename... Args>
typename std::enable_if<I < sizeof...(Args)>::type
print_tuple(const std::tuple<Args...>& t) {
    if (I > 0) std::cout << ".";
    std::cout << std::get<I>(t);
    print_tuple<I + 1>(t);
}

template <typename... Args>
void print_ip(const std::tuple<Args...>& t) {
    print_tuple(t);
    std::cout << std::endl;
}

int main() {
    print_ip( int8_t{-1} );
    print_ip( int16_t{0} );
    print_ip( int32_t{2130706433} );
    print_ip( int64_t{8875824491850138409} );
    print_ip( std::string{"Hello, World!"} );
    print_ip( std::vector<int>{100, 200, 300, 400} );
    print_ip( std::list<short>{400, 300, 200, 100} );
    print_ip( std::make_tuple(123, 456, 789, 0) );
    
    return 0;
}
