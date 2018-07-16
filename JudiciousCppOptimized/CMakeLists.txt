cmake_minimum_required(VERSION 3.10)
project(JudiciousCppOptimized)

set(CMAKE_CXX_STANDARD 14)

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

include_directories(.)

add_executable(JudiciousCpp
        Hypergraph.h
        main.cpp
        algorithms.cpp)

#add_executable(JudiciousCppDebugOn
#        Hypergraph.h
#        main.cpp
#        algorithms.cpp)


add_executable(unittest
        unittests.cpp
        algorithms.cpp)

target_link_libraries(unittest
        libgtest
        libgmock)