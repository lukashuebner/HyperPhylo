cmake_minimum_required(VERSION 3.10)
project(MinMaxPartitioning)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

include_directories(../JudiciousPartitioning/include)

add_executable(MinMaxPartitioning
        main.cpp
        ../JudiciousPartitioning/src/datastructures/Hypergraph.cpp
        ../JudiciousPartitioning/src/Helper.cpp)
