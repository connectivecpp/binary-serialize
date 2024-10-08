/** @file
 *
 * @brief This is an implementation of the C++ 23 @c std::byteswap function,
 * for use in pre C++ 23 applications.
 *
 * This implementation is taken directly from the CPP Reference page:
 * https://en.cppreference.com/w/cpp/numeric/byteswap
 *
 * @author Cliff Green, CPP Reference
 *
 * @copyright (c) 2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef BYTESWAP_HPP_INCLUDED
#define BYTESWAP_HPP_INCLUDED

#include <concepts> // std::integral concept
#include <bit> // std::bit_cast
#include <array>
#include <cstddef> // std::byte
#include <algorithm> // std::ranges::reverse

namespace chops {

/**
 * @brief Perform an in-place byte swap on an integral type (if size of the 
 * type is greater than one).
 *
 * @tparam T Type of value where swapping will occur.
 *
 * @param value Integral value to be byte swapped.
 *
 * @pre There must not be any padding bits in the type.
 *
 */
template<std::integral T>
constexpr T byteswap(T value) noexcept {
  if constexpr (sizeof(T) == 1u) {
    return value;
  }
  static_assert(std::has_unique_object_representations_v<T>,
                "T may not have padding bits");
  auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
  std::ranges::reverse(value_representation);
  return std::bit_cast<T>(value_representation);
}

} // end namespace

#endif

