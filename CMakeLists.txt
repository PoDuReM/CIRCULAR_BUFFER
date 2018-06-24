cmake_minimum_required(VERSION 3.10)
project(CIRCULAR_BUFFER)

set(CMAKE_CXX_STANDARD 11)

add_executable(CIRCULAR_BUFFER fault_injection.cpp fault_injection.h counted.cpp counted.h gtest/gtest-all.cc gtest/gtest.h gtest/gtest_main.cc circularbuffer.h testing.cpp counted.h counted.cpp fault_injection.h fault_injection.cpp)