cmake_minimum_required(VERSION 3.8)
project(inotify-cppExample)

###############################################################################
# INOTIFY-CPP
###############################################################################
if(NOT TARGET inotify-cpp::inotify-cpp)
    find_package(inotify-cpp CONFIG REQUIRED)
endif()

###############################################################################
# Boost
###############################################################################
find_package(
        Boost 1.54.0
        COMPONENTS unit_test_framework system
        REQUIRED
)

###############################################################################
# Thread
###############################################################################
find_package(Threads)

###############################################################################
# Target
###############################################################################
add_executable(inotify_example main.cpp)
target_compile_features(inotify_example PRIVATE cxx_std_17)
target_link_libraries(inotify_example
        PRIVATE
        inotify-cpp::inotify-cpp
        ${CMAKE_THREAD_LIBS_INIT})
