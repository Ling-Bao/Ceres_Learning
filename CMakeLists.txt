cmake_minimum_required(VERSION 2.8)
project(Ceres_Learning)

# Set Debug Mode
set(CMAKE_BUILD_TYPE "Debug")

# Set C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

# Set output directories for libraries and executables
SET(BASE_DIR ${CMAKE_SOURCE_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BASE_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BASE_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin)

# Add Dependency for Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

# Add Link Libraries
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Include Internel Include
include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCE_FILES main.cpp)
add_executable(Ceres_Learning ${SOURCE_FILES})
target_link_libraries(Ceres_Learning ${CERES_LIBRARIES})