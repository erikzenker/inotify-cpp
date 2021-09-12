cmake_minimum_required(VERSION 3.8)
project(inotify-cppExample)

###############################################################################
# INOTIFY-CPP
###############################################################################
if(NOT TARGET inotify-cpp::inotify-cpp)
    find_package(inotify-cpp CONFIG REQUIRED)
endif()

###############################################################################
# Thread
###############################################################################
find_package(Threads)

###############################################################################
# Target
###############################################################################
add_executable(inotify_example main.cpp)
target_link_libraries(inotify_example
        PRIVATE
        inotify-cpp::inotify-cpp
        ${CMAKE_THREAD_LIBS_INIT})
