cmake_minimum_required(VERSION 3.8)
project(inotify-cpp)

option(BUILD_EXAMPLE "Build inotify-cpp example program" ON)
option(BUILD_TEST "Build inotify-cpp unittest program" ON)
option(BUILD_SHARED_LIBS "Build inotify-cpp as a shared library" ON)
option(BUILD_STATIC_LIBS "Build inotify-cpp as a static library" OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(FATAL_ERROR "conanbuildinfo.cmake required is missing")
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

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
message(STATUS "  Build shared libs  .............. : ${BUILD_SHARED_LIBS}")
message(STATUS "  Build static libs  .............. : ${BUILD_STATIC_LIBS}")
message(STATUS "  Build example  .................. : ${BUILD_EXAMPLE}")
message(STATUS "  Build test ...................... : ${BUILD_TEST}")
message(STATUS "")
