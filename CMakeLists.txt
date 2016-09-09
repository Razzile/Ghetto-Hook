cmake_minimum_required(VERSION 2.8.7)
project(ghetto_hook C)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()


include_directories("${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE ghetto_SRC
   "src/*.c"
)

add_executable(ghetto_hook ${ghetto_SRC})
