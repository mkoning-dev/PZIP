cmake_minimum_required(VERSION 3.16)
project(PZip VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(include)

# Create a shared library (DLL) for PZip
add_library(pzip_lib SHARED ${SOURCES})

# Add main program for PZip
add_executable(pzip src/main.cpp)

# Link the executable to the shared library
target_link_libraries(pzip PRIVATE pzip_lib)

# Add tests (optional)
enable_testing()
add_subdirectory(tests)

# Add GoogleTest submodule
add_subdirectory(extern/googletest)

# Create the test executable
add_executable(EncoderTestExec tests/EncoderTest.cpp)

# Link the test executable to PZip and GoogleTest
target_link_libraries(EncoderTestExec PRIVATE gtest gtest_main pzip_lib)

# Enable C++11 or later for GoogleTest
target_compile_features(EncoderTestExec PRIVATE cxx_std_11)

# Include the GoogleTest directory
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
