# Binary Serialize, Header-Only C++ 20 Binary Serialization Classes and Functions Using a `std::format` Like Syntax

#### Unit Test and Documentation Generation Workflow Status

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/binary-serialize/build_run_unit_test_cmake.yml?branch=main&label=GH%20Actions%20build,%20unit%20tests%20on%20main)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/binary-serialize/build_run_unit_test_cmake.yml?branch=develop&label=GH%20Actions%20build,%20unit%20tests%20on%20develop)

![GH Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/connectivecpp/binary-serialize/gen_docs.yml?branch=main&label=GH%20Actions%20generate%20docs)

![GH Tag](https://img.shields.io/github/v/tag/connectivecpp/binary-serialize?label=GH%20tag)

![License](https://img.shields.io/badge/License-Boost%201.0-blue)

## Overview

The `binary_serialize` functions and classes provide serializing and unserializing of binary data. Serialization provides a way to transform application objects into and out of byte streams that can be sent over a network (or used for file IO). Many serialization libraries transform objects to and from text representations, but this library keeps data in binary formats.

The serialization functionality in this repository is useful when explicit control is needed for every bit and byte. This allows a developer to match an existing wire protocol or encoding scheme or to define his or her own wire protocol. Support is provided for fundamental arithmetic types as well as certain C++ vocabulary types such as `std::optional`. Both big and little endian support is provided.

This library uses `std::format` style formatting. For example:

```
  struct Hike {
    unsigned int  distance;
    int  elevation;
    std::optional<std::string>> name;
    std::vector<int> waypoints;
  };
  // ...
  chops::mutable_shared_buffer buf;

  chops::binary_serialize(buf, "{32ui}{16i}{8ui}{16ui}{16ui}{64i}", 
                          hike_obj.distance, hike_obj.elevation, hike_obj.name, hike_obj.waypoints);

  // ...
  net_obj.send(buf);
                                                                   
```

The buffer will contain the following:

```
  32 bit unsigned integer containing distance value
  16 bit signed integer containing elevation value
  8 bit unsigned integer corresponding to true or false for the optional
  16 bit unsigned integer for the size of the name string
  0 - N 8 bit characters for the name string
  16 bit unsigned integer for the size of the waypoints vector
  0 - N 64 bit signed integers for each waypoint value
```

The documentation overview provides a comparison with other serialization libraries as well as a rationale for the design decisions.

Inspiration and thanks go to [Louis Langholtz](https://github.com/louis-langholtz), who steered me towards considering the `std::format` API.

## Generated Documentation

The generated Doxygen documentation for `binary_serialize` is [here](https://connectivecpp.github.io/binary-serialize/).

## Dependencies

The `binary_serialize` header files do not have any third-party dependencies. It uses C++ standard library headers only. The unit test code does have dependencies as noted below.

## C++ Standard

`binary_serialize`  uses C++ 20 features, including  ... (fill in details here) ... the "spaceship" operator (`<=>`), `std::span`, and `concepts` / `requires`.

## Supported Compilers

Continuous integration workflows build and unit test on g++ (through Ubuntu), MSVC (through Windows), and clang (through macOS).

## Unit Test Dependencies

The unit test code uses [Catch2](https://github.com/catchorg/Catch2). If the `BINARY_SERIALIZE_BUILD_TESTS` flag is provided to Cmake (see commands below) the Cmake configure / generate will download the Catch2 library as appropriate using the [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) dependency manager. If Catch2 (v3 or greater) is already installed using a different package manager (such as Conan or vcpkg), the `CPM_USE_LOCAL_PACKAGES` variable can be set which results in `find_package` being attempted. Note that v3 (or later) of Catch2 is required.

The unit test uses utilities from Connective C++'s [utility-rack](https://github.com/connectivecpp/utility-rack).

Specific version (or branch) specs for the dependencies are in the [test/CMakeLists.txt](test/CMakeLists.txt) file, look for the `CPMAddPackage` commands.

## Build and Run Unit Tests

To build and run the unit test program:

First clone the `binary-serialize` repository, then create a build directory in parallel to the `binary-serialize` directory (this is called "out of source" builds, which is recommended), then `cd` (change directory) into the build directory. The CMake commands:

```
cmake -D BINARY_SERIALIZE_BUILD_TESTS:BOOL=ON ../binary-serialize

cmake --build .

ctest
```

For additional test output, run the unit test individually, for example:

```
test/binary_serialize_test -s
```

The example can be built by adding `-D BINARY_SERIALIZE_BUILD_EXAMPLES:BOOL=ON` to the CMake configure / generate step.

