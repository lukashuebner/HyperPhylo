cmake_minimum_required(VERSION 3.10)
project(JudiciousPartitioning)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -DNDEBUG")
set(OPENMP_ENABLED TRUE CACHE BOOLEAN "Openmp enabled or not")

execute_process(COMMAND cat /proc/cpuinfo OUTPUT_VARIABLE result)
if ("${result}" MATCHES "avx2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
endif()

if ("${result}" MATCHES "avx512f")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=skylake-avx512")
endif()

find_package(OpenMP)

# Change to TRUE to activate OpenMP
if (OPENMP_FOUND AND "${OPENMP_ENABLED}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include_directories(include /opt/intel/tbb/include)
link_directories(/opt/intel/tbb/lib/intel64/gcc4.7)
file(GLOB_RECURSE ds src/datastructures/*.cpp)

message("SET C FLAGS: ${CMAKE_C_FLAGS}")
message("SET CXX FLAGS: ${CMAKE_CXX_FLAGS}")

add_executable(JudiciousPartitioning
        ${ds}
        src/main.cpp
        src/Algorithms.cpp
        src/Helper.cpp)

add_executable(benchmarks
        ${ds}
        src/Benchmark.cpp
        src/Helper.cpp)

target_link_libraries(JudiciousPartitioning tbb)
target_link_libraries(benchmarks benchmark sdsl)