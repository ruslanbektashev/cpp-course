#pragma once

#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

namespace ip_printer::detail {

template <typename T>
using decay_t = std::decay_t<T>;

template <typename T>
inline constexpr bool is_string_v = std::is_same_v<decay_t<T>, std::string>;

template <typename T>
struct is_std_vector : std::false_type {};

template <typename... Args>
struct is_std_vector<std::vector<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<decay_t<T>>::value;

template <typename T>
struct is_std_list : std::false_type {};

template <typename... Args>
struct is_std_list<std::list<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_list_v = is_std_list<decay_t<T>>::value;

template <typename T>
inline constexpr bool is_list_or_vector_v = is_std_list_v<T> || is_std_vector_v<T>;

template <typename T>
inline constexpr bool is_integral_v =
    std::is_integral_v<decay_t<T>> && !std::is_same_v<decay_t<T>, bool>;

template <typename T>
void print_dotted_range(std::ostream& os, const T& range)
{
  auto it = std::begin(range);
  const auto end = std::end(range);
  if (it == end) {
    return;
  }

  os << *it;
  ++it;
  for (; it != end; ++it) {
    os << '.' << *it;
  }
}

} // namespace ip_printer::detail

// 1) Integral types: print bytes from MSB to LSB (unsigned), separated by '.'
template <typename T, std::enable_if_t<ip_printer::detail::is_integral_v<T>, int> = 0>
void print_ip(const T& value, std::ostream& os = std::cout)
{
  using U = std::make_unsigned_t<ip_printer::detail::decay_t<T>>;
  const U uvalue = static_cast<U>(value);

  for (std::size_t i = 0; i < sizeof(U); ++i) {
    const std::size_t shift = (sizeof(U) - 1 - i) * 8;
    const auto byte = static_cast<std::uint8_t>((uvalue >> shift) & 0xFFu);
    if (i != 0) {
      os << '.';
    }
    os << static_cast<unsigned>(byte);
  }
}

// 2) std::string: print as-is
template <typename T, std::enable_if_t<ip_printer::detail::is_string_v<T>, int> = 0>
void print_ip(const T& value, std::ostream& os = std::cout)
{
  os << value;
}

// 3) std::vector / std::list: print elements separated by '.'
template <typename T, std::enable_if_t<ip_printer::detail::is_list_or_vector_v<T>, int> = 0>
void print_ip(const T& value, std::ostream& os = std::cout)
{
  ip_printer::detail::print_dotted_range(os, value);
}
