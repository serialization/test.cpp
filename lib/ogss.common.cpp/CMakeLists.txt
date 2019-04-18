cmake_minimum_required(VERSION 2.8.2)
project(ogss_common_cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")


################################
# Build common lib
################################

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES false ogss/*.cpp)

# The resulting library to be used by generated code
ADD_LIBRARY(ogss.common.cpp STATIC ${SOURCE_FILES})

set_property(TARGET ogss.common.cpp PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
