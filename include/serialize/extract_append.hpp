/** @file
 *
 * @brief Functions to extract arithmetic binary values from a byte buffer (in either
 * endian order) to native format; conversely, given an arithmetic binary value, append 
 * it to a buffer of bytes in the specified endian order.
 *
 * The functions in this file are low-level. The handle fundamental arithmetic types and 
 * extracting or appending to @c std::byte buffers. It is meant to be the lower layer
 * of serializing utilities, where the next layer up provides buffer management,
 * sequences, and overloads for specific types such as @c std::string, @c bool, and 
 * @c std::optional.
 *
 * @note The variable sized integer functions (@c extract_var_int, @c append_var_int) 
 * support the variable byte integer type in MQTT (Message Queuing Telemetry Transport),
 * a commonly used IoT protocol. The code in this header is adapted from a 
 * Techoverflow.net article by Uli Koehler and published under the CC0 1.0 Universal 
 * license:
 * https://techoverflow.net/2013/01/25/efficiently-encoding-variable-length-integers-in-cc/
 *
 * @author Cliff Green, Roxanne Agerone, Uli Koehler
 *
 * @copyright (c) 2019-2024 by Cliff Green, Roxanne Agerone
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef EXTRACT_APPEND_HPP_INCLUDED
#define EXTRACT_APPEND_HPP_INCLUDED

#include "serialize/byteswap.hpp"

#include <concepts> // std::unsigned_integral, std::integral
#include <algorithm> // std::ranges:copy
#include <bit> // std::endian, std::bit_cast
#include <array>
#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint32_t, etc
#include <type_traits> // std::is_same


namespace chops {

// Old static assertions, pre-concepts
/*
template <typename T>
constexpr void assert_size() noexcept {
  static_assert(sizeof(T) == 1u || sizeof(T) == 2u || sizeof(T) == 4u || 
                sizeof(T) == 8u || sizeof(T) ==16u,
    "Size for value extraction is not supported.");
}
template <typename T>
constexpr bool is_arithmetic_or_byte() noexcept {
  return std::is_arithmetic_v<T> || std::is_same_v<std::remove_cv_t<T>, std::byte>;
}

template <typename T>
constexpr void assert_arithmetic_or_byte() noexcept {
  static_assert(is_arithmetic_or_byte<T>(),
    "Value extraction is only supported for arithmetic or std::byte types.");
}
*/

// using C++ 20 concepts
template <typename T>
concept integral_or_byte = std::integral<T> || std::is_same_v<std::remove_cv_t<T>, std::byte>;

// old endian detection code
// inline bool detect_big_endian () noexcept {
//  const std::uint32_t tmp {0xDDCCBBAA};
//  return *(cast_ptr_to<std::byte>(&tmp) + 3) == static_cast<std::byte>(0xAA);
//}
// should be computed at global initialization time
// const bool big_endian = detect_big_endian();

/**
 * @brief Extract a value from a @c std::byte buffer into a fundamental integral
 * or @c std::byte type in native endianness, swapping bytes as needed.
 *
 * @tparam BufEndian The endianness of the buffer.
 *
 * @tparam T Type of return value.
 *
 * Since @c T cannot be deduced, it must be specified when calling the function. If
 * the endianness of the buffer matches the native endianness, no swapping is performed.
 *
 * @param buf Pointer to an array of @c std::bytes containing an object of type T in network 
 * byte order.
 *
 * @return A value in native endian order.
 *
 * @pre The buffer must contain at least @c sizeof(T) bytes.
 *
 * @note Floating point swapping is not supported.
 *
 * Earlier versions did support floating point, but it is brittle - 
 * the floating point representation must exactly match on both sides of the serialization
 * (most modern processors use IEEE 754 floating point representations). A byte swapped
 * floating point value cannot be directly accessed (e.g. passed by value), due to the
 * bit patterns possibly representing NaN values, which can generate hardware traps,
 * either causing runtime crashes or silently changing bits within the floating point number.
 * An integer value, however, will always have valid bit patterns, even when byte swapped.
 *
 */
template <std::endian BufEndian, integral_or_byte T>
constexpr T extract_val(const std::byte* buf) noexcept {
  auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(T{});
  std::ranges::copy (buf, buf + sizeof(T), value_representation.begin());
  auto tmp_val = std::bit_cast<T>(value_representation);
  if constexpr (BufEndian != std::endian::native && sizeof(T) != 1u) {
    return chops::byteswap(tmp_val);
  }
  return tmp_val;
}

/**
 * @brief Append an integral or @c std::byte value to a @c std::byte buffer, swapping into
 * the specified endian order as needed.
 *
 * @tparam BufEndian The endianness of the buffer.
 *
 * @tparam T Type of value to append to buffer.
 *
 * The @c BufEndian enum must be specified, but the type of the passed in value can be
 * deduced.
 *
 * @param buf Pointer to array of @c std::bytes big enough to hold the bytes of the value.
 *
 * @param val Value in native endian order to append to buf.
 *
 * @return Number of bytes copied into the @c std::byte buffer.
 *
 * @pre The buffer must already be allocated to hold at least @c sizeof(T) bytes.
 *
 * @note See note above about floating point values.
 *
 */
template <std::endian BufEndian, integral_or_byte T>
constexpr std::size_t append_val(std::byte* buf, const T& val) noexcept {
  T tmp_val {val};
  if constexpr (BufEndian != std::endian::native && sizeof(T) != 1u) {
    tmp_val = chops::byteswap(tmp_val);
  }
  auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(tmp_val);
  std::ranges::copy (value_representation, buf );
  return sizeof(T);
}

/**
 * @brief Encode an unsigned integer into a variable length buffer of bytes using the MSB (most
 * significant bit) algorithm.
 *
 * Given an integer, store the value in 1 or 2 or 3 or more bytes depending on the
 * value. If small (under 128) the value is stored in one byte. If the value is greater
 * than 127, the most significant bit in the first byte is set to 1 and the value is stored
 * in two bytes. This logic is repeated as necessary.
 *
 * This algorithm optimizes space when most of the values are small. If most of the values are
 * large, this algorithm is inefficient, needing more buffer space for the encoded integers than
 * if fixed size integer buffers were used.
 *
 * The output of this function is (by definition) in little-endian order. However, as long as
 * the two corresponding functions (or equivalent algorithms) are used consistently, the
 * endianness will not matter. There is no byte swapping performed, and encoding and decoding 
 * will result in the native endianness of the platform. I.e. this works whether serialization 
 * is big-endian or little-endian.
 * 
 * @note Unsigned types are not supported.
 *
 * @param val The input value. Any standard unsigned integer type is allowed.
 *
 * @param output A pointer to a preallocated array of @c std::bytes big enough for the output.
 * A safe minimum size is 5 bytes for 32 bit integers, 10 bytes for 64 bit integers and 3 bytes
 * for 16 bit integers.
 *
 * @return The number of bytes written to the output array.
 * 
 * @pre The output buffer must already be allocated large enough to hold the result.
 *
 */

template<std::unsigned_integral T>
constexpr std::size_t append_var_int(std::byte* output, T val) {

  std::size_t output_size = 0;
    
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (val > 127) {

      output[output_size] = std::bit_cast<std::byte>(static_cast<std::uint8_t>((static_cast<std::uint8_t> (val & 127)) | 128));
      //Remove the seven bits we just wrote
      val >>= 7;
      ++output_size;
  }
  output[output_size++] = std::bit_cast<std::byte>(static_cast<std::uint8_t>(static_cast<std::uint8_t> (val) & 127));
  return output_size;
}
/**
 * @brief Given a buffer of @c std::bytes that hold a variable sized integer, decode
 * them into an unsigned integer.
 *
 * For consistency with the @c append_var_int function, only unsigned integers are
 * supported for the output type of this function.
 *
 * @param input A variable-length encoded integer stored in a buffer of @c std::bytes.
 *
 * @param input_size Number of bytes representing the integer.
 *
 * @return The value in native unsigned integer format.
 * 
 */
template<std::unsigned_integral T>
constexpr T extract_var_int(const std::byte* input, std::size_t input_size) {
  T ret = 0;
  for (std::size_t i = 0; i < input_size; ++i) {
      ret |= (std::bit_cast<std::uint8_t>(input[i]) & 127) << (7 * i);
      //If the next-byte flag is set
      if(!(std::bit_cast<std::uint8_t>(input[i]) & 128)) {
          break;
      }
  }
  return ret;
}

} // end namespace

#endif

