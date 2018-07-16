cmake_minimum_required(VERSION 3.10)
project(JudiciousCppOptimized)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)
pkg_check_modules(GMOCK REQUIRED gmock>=1.7.0)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
)

include_directories(.)

add_executable(JudiciousCppOptimized
        Hypergraph.h
        main.cpp
        algorithms.cpp)

add_executable(unittest
        unittests.cpp
        algorithms.cpp)

target_link_libraries(unittest
        ${GTEST_LIBRARIES}
        ${GMOCK_LIBRARIES}
)
