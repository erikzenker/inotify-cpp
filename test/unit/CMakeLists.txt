cmake_minimum_required(VERSION 3.5)
project(inotify-cppUnitTests)

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
        COMPONENTS unit_test_framework system filesystem
        REQUIRED
)

###############################################################################
# Thread
###############################################################################
find_package(Threads)

###############################################################################
# Test
###############################################################################
add_executable(inotify_unit_test main.cpp NotifierBuilderTests.cpp EventTests.cpp)
target_link_libraries(inotify_unit_test
        PRIVATE
          inotify-cpp::inotify-cpp
          Boost::unit_test_framework
          Boost::system
          Boost::filesystem
          ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME inotify_unit_test COMMAND inotify_unit_test)
