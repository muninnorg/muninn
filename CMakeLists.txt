# Project name
project (muninn)

# Set version number
set (VERSION 0.412)

# Minimum required version of cmake
cmake_minimum_required (VERSION 2.6.2)

# Set default make type
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

# Set parameters for different build types
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")

# Require proper C++ code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -pedantic -Wno-long-long -Wno-enum-compare")

# Create all libraries in lib
set(LIBRARY_OUTPUT_PATH ${muninn_BINARY_DIR}/libs)

# Add root directories to the list of includes
include_directories (${muninn_SOURCE_DIR} ${muninn_SOURCE_DIR}/external/) 

# Add sub-directories
add_subdirectory(muninn)
add_subdirectory(bin)
