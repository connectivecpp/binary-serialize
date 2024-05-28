/** @file
 *
 * @brief Test scenarios for @c byteswap function.
 *
 * @author Cliff Green
 *
 * @copyright (c) 2024 by Cliff Green
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch_test_macros.hpp"


#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t, etc

#include "serialize/byteswap.hpp"

constexpr std::uint32_t val1          { 0xDDCCBBAAu };
constexpr std::uint32_t val1_reversed { 0xAABBCCDDu };
constexpr char val2                   { static_cast<char>(0xEE) };
constexpr std::int16_t val3           { 0x0103 };
constexpr std::int16_t val3_reversed  { 0x0301 };
constexpr std::uint64_t val4          { 0x0908070605040302ull };
constexpr std::uint64_t val4_reversed { 0x0203040506070809ull };

constexpr std::int32_t val5           = 0xDEADBEEF;
constexpr std::int32_t val5_reversed  = 0xEFBEADDE;


TEST_CASE ( "Byteswap", "[byteswap]" ) {
  REQUIRE (chops::byteswap(val1) == val1_reversed);
  REQUIRE (chops::byteswap(val2) == val2);
  REQUIRE (chops::byteswap(val3) == val3_reversed);
  REQUIRE (chops::byteswap(val4) == val4_reversed);
  REQUIRE (chops::byteswap(val5) == val5_reversed);
}

