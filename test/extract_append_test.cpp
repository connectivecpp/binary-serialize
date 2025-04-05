/** @file
 *
 * @brief Test scenarios for @c extract_val and @c append_val functions.
 *
 * @author Cliff Green, Roxanne Agerone
 *
 * @copyright (c) 2019-2025 by Cliff Green, Roxanne Agerone
 *
 * Distributed under the Boost Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch_test_macros.hpp"


#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t, etc
#include <ranges> // std::views::iota

#include "serialize/extract_append.hpp"

#include "utility/byte_array.hpp"

constexpr std::uint32_t val1 = 0xDDCCBBAA;
constexpr char val2 = static_cast<char>(0xEE);
constexpr std::int16_t val3 = 0x01FF;
constexpr std::uint64_t val4 = 0x0908070605040302;
constexpr std::int32_t val5 = 0xDEADBEEF;
constexpr std::byte val6 = static_cast<std::byte>(0xAA);

constexpr int arr_sz = sizeof(val1)+sizeof(val2)+sizeof(val3)+
                       sizeof(val4)+sizeof(val5)+sizeof(val6);

auto net_buf_big = chops::make_byte_array(0xDD, 0xCC, 0xBB, 0xAA, 0xEE, 0x01, 0xFF,
    0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0xDE, 0xAD, 0xBE, 0xEF, 0xAA);
auto net_buf_little = chops::make_byte_array(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01,
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xEF, 0xBE, 0xAD, 0xDE, 0xAA);

TEST_CASE ( "Append values into a buffer", "[append_val]" ) {

  std::byte buf[arr_sz];
  constexpr std::uint32_t v = 0x04030201;

  SECTION ("Append_val with a single value, big endian") {
    REQUIRE(chops::append_val<std::endian::big>(buf, v) == 4u);
    REQUIRE (std::to_integer<int>(buf[0]) == 0x04);
    REQUIRE (std::to_integer<int>(buf[1]) == 0x03);
    REQUIRE (std::to_integer<int>(buf[2]) == 0x02);
    REQUIRE (std::to_integer<int>(buf[3]) == 0x01);
  }
  SECTION ("Append_val with a single value, little endian") {
    REQUIRE(chops::append_val<std::endian::little>(buf, v) == 4u);
    REQUIRE (std::to_integer<int>(buf[0]) == 0x01);
    REQUIRE (std::to_integer<int>(buf[1]) == 0x02);
    REQUIRE (std::to_integer<int>(buf[2]) == 0x03);
    REQUIRE (std::to_integer<int>(buf[3]) == 0x04);
  }

  SECTION ("Append_val with multiple values, big endian") {
    std::byte* ptr = buf;
    REQUIRE(chops::append_val<std::endian::big>(ptr, val1) == 4u); ptr += sizeof(val1);
    REQUIRE(chops::append_val<std::endian::big>(ptr, val2) == 1u); ptr += sizeof(val2);
    REQUIRE(chops::append_val<std::endian::big>(ptr, val3) == 2u); ptr += sizeof(val3);
    REQUIRE(chops::append_val<std::endian::big>(ptr, val4) == 8u); ptr += sizeof(val4);
    REQUIRE(chops::append_val<std::endian::big>(ptr, val5) == 4u); ptr += sizeof(val5);
    REQUIRE(chops::append_val<std::endian::big>(ptr, val6) == 1u);
    for (int i : std::views::iota(0, arr_sz)) {
      REQUIRE (std::to_integer<int>(buf[i]) == std::to_integer<int>(net_buf_big[i]));
    }
  }
  SECTION ("Append_val with multiple values, little endian") {
    std::byte* ptr = buf;
    REQUIRE(chops::append_val<std::endian::little>(ptr, val1) == 4u); ptr += sizeof(val1);
    REQUIRE(chops::append_val<std::endian::little>(ptr, val2) == 1u); ptr += sizeof(val2);
    REQUIRE(chops::append_val<std::endian::little>(ptr, val3) == 2u); ptr += sizeof(val3);
    REQUIRE(chops::append_val<std::endian::little>(ptr, val4) == 8u); ptr += sizeof(val4);
    REQUIRE(chops::append_val<std::endian::little>(ptr, val5) == 4u); ptr += sizeof(val5);
    REQUIRE(chops::append_val<std::endian::little>(ptr, val6) == 1u);
    for (int i : std::views::iota(0, arr_sz)) {
      REQUIRE (std::to_integer<int>(buf[i]) == std::to_integer<int>(net_buf_little[i]));
    }
  }
}

TEST_CASE ( "Extract values from a buffer", "[extract_val]" ) {

  SECTION ( "Extract_val for multiple values in big endian buf") {
    const std::byte* ptr = net_buf_big.data();
    std::uint32_t v1 = chops::extract_val<std::endian::big, std::uint32_t>(ptr); ptr += sizeof(v1);
    char v2 = chops::extract_val<std::endian::big, char>(ptr); ptr += sizeof(v2);
    std::int16_t v3 = chops::extract_val<std::endian::big, std::int16_t>(ptr); ptr += sizeof(v3);
    std::uint64_t v4 = chops::extract_val<std::endian::big, std::uint64_t>(ptr); ptr += sizeof(v4);
    std::int32_t v5 = chops::extract_val<std::endian::big, std::int32_t>(ptr); ptr += sizeof(v5);
    std::byte v6 = chops::extract_val<std::endian::big, std::byte>(ptr);

    REQUIRE(v1 == val1);
    REQUIRE(v2 == val2);
    REQUIRE(v3 == val3);
    REQUIRE(v4 == val4);
    REQUIRE(v5 == val5);
    REQUIRE(std::to_integer<int>(v6) == std::to_integer<int>(val6));
  }
  SECTION ( "Extract_val for multiple values in little endian buf") {
    const std::byte* ptr = net_buf_little.data();
    std::uint32_t v1 = chops::extract_val<std::endian::little, std::uint32_t>(ptr); ptr += sizeof(v1);
    char v2 = chops::extract_val<std::endian::little, char>(ptr); ptr += sizeof(v2);
    std::int16_t v3 = chops::extract_val<std::endian::little, std::int16_t>(ptr); ptr += sizeof(v3);
    std::uint64_t v4 = chops::extract_val<std::endian::little, std::uint64_t>(ptr); ptr += sizeof(v4);
    std::int32_t v5 = chops::extract_val<std::endian::little, std::int32_t>(ptr); ptr += sizeof(v5);
    std::byte v6 = chops::extract_val<std::endian::little, std::byte>(ptr);

    REQUIRE(v1 == val1);
    REQUIRE(v2 == val2);
    REQUIRE(v3 == val3);
    REQUIRE(v4 == val4);
    REQUIRE(v5 == val5);
    REQUIRE(std::to_integer<int>(v6) == std::to_integer<int>(val6));
  }
}


template <typename Dest, typename Src>
void test_round_trip_var_int (Src src, std::size_t exp_sz) {
  std::byte test_buf [10];
  auto outsize = chops::append_var_int<Src>(test_buf, src);
  auto output = chops::extract_var_int<Dest>(test_buf, outsize);
  REQUIRE(output == src);
  REQUIRE(outsize == exp_sz);
}

TEST_CASE ( "Append and extract variable length integers","[append_var_int]" ) {

    std::byte test_buf [10];

  {
    auto outsize = chops::append_var_int<std::uint32_t>(test_buf, 0xCAFE);
    REQUIRE(std::to_integer<int>(test_buf[0]) == 254);
    REQUIRE(std::to_integer<int>(test_buf[1]) == 149);
    REQUIRE(std::to_integer<int>(test_buf[2]) == 3);

    auto output = chops::extract_var_int<unsigned int>(test_buf, outsize);
    
    REQUIRE(output == 51966u); // 0xCAFE is 51966
    REQUIRE(outsize == 3u);
  }
}

TEST_CASE ( "Append and extract variable length integers, round trip testing",
            "[append_var_int]" ) {

  test_round_trip_var_int<unsigned int> (static_cast<std::uint32_t>(0xFFFFFFFF),
                                         5u);
  test_round_trip_var_int<unsigned short> (static_cast<std::uint16_t>(40001u),
                                         3u);
  test_round_trip_var_int<unsigned short> (static_cast<std::uint16_t>(0xFFFF),
                                         3u);
  test_round_trip_var_int<unsigned short> (static_cast<std::uint16_t>(7u),
                                         1u);
  test_round_trip_var_int<unsigned long long> (static_cast<std::uint64_t>(0xFFFFFFFFFFFFFFFF),
                                         10u);
  test_round_trip_var_int<unsigned int> (static_cast<std::uint32_t>(42u),
                                         1u);
}

TEST_CASE ( "Append var len integer of 127","[append_var_int]" ) {
    
    std::byte test_buf [7];
    auto outsize = chops::append_var_int<unsigned int>(test_buf, 0x7F);
    REQUIRE(std::to_integer<int>(test_buf[0]) == 127);
    REQUIRE(outsize == 1);
}
TEST_CASE ( "Append var len integer of 128","[append_var_int]" ) {
    
    std::byte test_buf [7];
    auto outsize = chops::append_var_int<unsigned int>(test_buf, 0x80);
    REQUIRE(std::to_integer<int>(test_buf[0]) == 128); //byte flag set
    REQUIRE(std::to_integer<int>(test_buf[1]) == 1);
    REQUIRE(outsize == 2);
}
TEST_CASE ( "Append var len integer larger than 4 bytes","[append_var_int]" ) {
    
    std::byte test_buf [7];
    auto outsize = chops::append_var_int<unsigned int>(test_buf, 0x10000000);
    REQUIRE(std::to_integer<int>(test_buf[0]) == 128); //byte flag set
    REQUIRE(std::to_integer<int>(test_buf[4]) == 1);
    REQUIRE(outsize == 5);
}

TEST_CASE( "Extracting variable integer larger than two bytes", "[extract_var_int]" ) {
    std::byte test_buf [7];
    test_buf[0] = static_cast<std::byte>(0xFE); // encodes to 126 with byte flag set
    test_buf[1] = static_cast<std::byte>(0xCA); // encodes to 9472
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 2u);
    // 126 + 9472 = 9598
    REQUIRE( val1 == 9598 );
}

TEST_CASE("Extracting variable integer smaller than a byte", "[extract_var_int]"){
    std::byte test_buf [7];
    test_buf[0] = static_cast<std::byte>(0x7F); // encodes to 127
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 1u);
    REQUIRE( val1 == 127 );
}

TEST_CASE("Extracting variable integer of 128", "[extract_var_int]"){
    std::byte test_buf [7];
    test_buf[0] = static_cast<std::byte>(0x80); // encodes to 0 with byte flag set
    test_buf[1] = static_cast<std::byte>(0x01); // encodes to 128
    auto val1 = chops::extract_var_int<std::uint32_t>(test_buf, 2u);
    // 127 + 1 = 128
    REQUIRE( val1 == 128 );
}

