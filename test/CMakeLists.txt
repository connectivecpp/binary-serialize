# Copyright (c) 2024 by Cliff Green
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

# create project
project ( binary_serialize_test LANGUAGES CXX )

# add executable
add_executable ( binary_serialize_test binary_serialize_test.cpp )
target_compile_features ( binary_serialize_test PRIVATE cxx_std_20 )

# add dependencies
include ( ../cmake/download_cpm.cmake )

CPMAddPackage ( "gh:catchorg/Catch2@3.5.4" )
CPMAddPackage ( "gh:connectivecpp/utility-rack@1.0.0" )

# link dependencies
target_link_libraries ( binary_serialize_test PRIVATE utility_rack Catch2::Catch2WithMain )

enable_testing()

add_test ( NAME run_binary_serialize_test COMMAND binary_serialize_test )
set_tests_properties ( run_binary_serialize_test 
  PROPERTIES PASS_REGULAR_EXPRESSION "All tests passed"
  )

