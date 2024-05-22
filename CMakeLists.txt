# Copyright (c) 2024 by Cliff Green
#
# https://github.com/connectivecpp/binary-serialize
#
# I'm still learning CMake, so improvement suggestions are always welcome.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

project ( binary_serialize 
	  LANGUAGES CXX 
	  DESCRIPTION "Binary serialization with std::format like syntax"
	  HOMEPAGE_URL "https://github.com/connectivecpp/binary-serialize/" )

  option ( BINARY_SERIALIZE_BUILD_TESTS "Build unit tests" OFF )
  option ( BINARY_SERIALIZE_BUILD_EXAMPLES "Build examples" OFF )
  option ( BINARY_SERIALIZE_INSTALL "Install header only library" OFF )

# add library targets

add_library ( binary_serialize INTERFACE )
add_library ( chops::binary_serialize ALIAS binary_serialize )

# configure library target

target_include_directories ( binary_serialize INTERFACE
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
			     $<INSTALL_INTERFACE:include/> )
target_compile_features ( binary_serialize INTERFACE cxx_std_20 )

# check to build unit tests
if ( ${BINARY_SERIALIZE_BUILD_TESTS} )
  enable_testing()
  add_subdirectory ( test )
endif ()

# check to build example code
if ( ${BINARY_SERIALIZE_BUILD_EXAMPLES} )
  add_subdirectory ( example )
endif ()

# check to install
if ( ${BINARY_SERIALIZE_INSTALL} )
  set ( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt )
  include ( CPack )
endif ()

# end of file

