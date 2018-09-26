cmake_minimum_required(VERSION 3.10)
project(JudiciousCppOptimized)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

execute_process(COMMAND cat /proc/cpuinfo OUTPUT_VARIABLE result)
if ("${result}" MATCHES "avx2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
endif()

if ("${result}" MATCHES "avx512f")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=skylake-avx512")
endif()

find_package(OpenMP)

# Change to TRUE to activate OpenMP
if (OPENMP_FOUND AND TRUE)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# We need thread support for googletest
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
        gtest
        URL https://github.com/google/googletest/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        # Disable install step
        INSTALL_COMMAND ""
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

include_directories("${source_dir}/googletest/include"
        "${source_dir}/googlemock/include")

#find_package(PkgConfig)
#pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)
#pkg_check_modules(GMOCK REQUIRED gmock>=1.7.0)

#include_directories(
#    ${GTEST_INCLUDE_DIRS}
#    ${GMOCK_INCLUDE_DIRS}
#)

include_directories(. /opt/intel/tbb/include /home/peet/GitRepos/tlx)
link_directories(/opt/intel/tbb/lib/intel64/gcc4.7)

add_executable(JudiciousCpp
        main.cpp
        algorithms.cpp
        Helper.cpp
        structures.cpp)

add_executable(test
        structures.cpp
        Helper.cpp
        pooltest.cpp)

add_executable(unittest
        unittests.cpp
        algorithms.cpp
        structures.cpp)

add_executable(benchmarks
        Benchmark.cpp
        Helper.cpp
        structures.cpp)

target_link_libraries(JudiciousCpp tbb)

target_link_libraries(test tbb)

target_link_libraries(unittest
        libgtest
        libgmock
        tbb)

target_link_libraries(benchmarks benchmark sdsl)