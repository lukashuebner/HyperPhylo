cmake_minimum_required(VERSION 3.10)
project(JudiciousCpp)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(JudiciousCpp
        Hypergraph.h
        twiddle.h
        main.cpp
        twiddle.cpp)