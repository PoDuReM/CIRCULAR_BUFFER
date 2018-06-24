cmake_minimum_required(VERSION 3.5.1)
project(buffer_cycle)

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

add_executable(buffer_cycle main.cpp circularbuffer.h)

add_executable(test_deque
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        counted.cpp
        counted.h
        fault_injection.cpp
        fault_injection.h
        tests.cpp
        circularbuffer.h)

target_link_libraries(test_deque -lpthread)
