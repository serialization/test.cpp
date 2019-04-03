cmake_minimum_required(VERSION 2.8.2)
project(ogss_common_cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -funroll-loops -Og -Wall -pedantic")


################################
# OpenMP
################################

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


################################
# Build common lib
################################

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false ogss/*.cpp)

# The resulting library to be used by generated code
ADD_LIBRARY(ogss.common.cpp STATIC ${SOURCE_FILES})

set_property(TARGET ogss.common.cpp PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
