#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <list>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace detail {

template <typename T>
struct is_std_vector : std::false_type {};

template <typename T, typename Allocator>
struct is_std_vector<std::vector<T, Allocator>> : std::true_type {};

template <typename T>
struct is_std_list : std::false_type {};

template <typename T, typename Allocator>
struct is_std_list<std::list<T, Allocator>> : std::true_type {};

template <typename T>
struct is_supported_container
    : std::bool_constant<is_std_vector<T>::value || is_std_list<T>::value> {};

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template <typename... Ts>
struct all_same : std::true_type {};

template <typename T, typename U, typename... Rest>
struct all_same<T, U, Rest...>
    : std::bool_constant<std::is_same_v<T, U> && all_same<U, Rest...>::value> {};

template <typename Tuple>
struct tuple_all_same;

template <typename... Ts>
struct tuple_all_same<std::tuple<Ts...>> : all_same<Ts...> {};

template <typename Tuple, std::size_t... Indices>
void print_tuple_impl(const Tuple& tuple, std::index_sequence<Indices...>) {
    ((std::cout << (Indices == 0 ? "" : ".") << std::get<Indices>(tuple)), ...);
    std::cout << '\n';
}

}  // namespace detail

template <typename T, std::enable_if_t<std::is_integral_v<std::decay_t<T>>, int> = 0>
void print_ip(T value) {
    using Unsigned = std::make_unsigned_t<std::decay_t<T>>;
    constexpr std::size_t byte_count = sizeof(Unsigned);
    const Unsigned as_unsigned = static_cast<Unsigned>(value);

    for (std::size_t i = 0; i < byte_count; ++i) {
        const std::size_t shift = (byte_count - 1 - i) * 8;
        const auto octet = static_cast<unsigned>((as_unsigned >> shift) & static_cast<Unsigned>(0xFF));
        if (i > 0) {
            std::cout << '.';
        }
        std::cout << octet;
    }
    std::cout << '\n';
}

template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, int> = 0>
void print_ip(const T& value) {
    std::cout << value << '\n';
}

template <typename T, std::enable_if_t<detail::is_supported_container<std::decay_t<T>>::value, int> = 0>
void print_ip(const T& container) {
    bool first = true;
    for (const auto& item : container) {
        if (!first) {
            std::cout << '.';
        }
        std::cout << item;
        first = false;
    }
    std::cout << '\n';
}

template <typename T, std::enable_if_t<detail::is_tuple<std::decay_t<T>>::value, int> = 0>
void print_ip(const T& tuple) {
    using TupleType = std::decay_t<T>;
    static_assert(detail::tuple_all_same<TupleType>::value, "All tuple element types must be identical");
    detail::print_tuple_impl(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>{});
}
