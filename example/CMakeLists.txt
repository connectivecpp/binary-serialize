# Copyright (c) 2024-2025 by Cliff Green
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

# create project
project ( binary_serialize_example LANGUAGES CXX )

# add executable
add_executable ( binary_serialize_example binary_serialize_example.cpp )
target_compile_features ( binary_serialize_example PRIVATE cxx_std_20 )

# link dependencies
target_link_libraries ( binary_serialize_example PRIVATE binary_serialize )

