cmake_minimum_required(VERSION 3.8)
project(inotify-cpp)

option(BUILD_EXAMPLE "Build inotify-cpp example program" ON)
option(BUILD_TEST "Build inotify-cpp unittest program" ON)

add_subdirectory(src)

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

message(STATUS "")
message(STATUS "")
message(STATUS "${PROJECT_NAME} configuration summary:")
message(STATUS "")
message(STATUS "  CMake build type ................ : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build example  .................. : ${BUILD_EXAMPLE}")
message(STATUS "  Build test ................. : ${BUILD_TEST}")
message(STATUS "")