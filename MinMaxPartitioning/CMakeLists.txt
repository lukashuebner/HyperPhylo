cmake_minimum_required(VERSION 3.10)
project(MinMaxPartitioning)

set(CMAKE_CXX_STANDARD 14)

include_directories(../JudiciousCppOptimized/include)

add_executable(MinMaxPartitioning main.cpp ../JudiciousCppOptimized/src/datastructures/Hypergraph.cpp)