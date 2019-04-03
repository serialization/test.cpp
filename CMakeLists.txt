cmake_minimum_required(VERSION 2.8.2)
project(ogss_test_cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")

################################
# OGSS/C++ common
################################
ADD_SUBDIRECTORY(lib/ogss.common.cpp)
include_directories(lib/ogss.common.cpp)

################################
# GTest
################################
ADD_SUBDIRECTORY(lib/gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# OpenMP
################################
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


################################
# Unit Tests
################################

#implementation
file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false src/*.cpp)
#actual tests
file(GLOB_RECURSE TEST_FILES LIST_DIRECTORIES false test/*.cpp)

add_executable(runTests ${SOURCE_FILES} ${TEST_FILES})

# Link test executable against gtest & gtest_main
target_link_libraries(runTests ogss.common.cpp gtest gtest_main)
add_test(runTests runTests)
