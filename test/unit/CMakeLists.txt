cmake_minimum_required(VERSION 3.8)
project(inotify-cppUnitTests)

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
# Test
###############################################################################
add_executable(inotify_unit_test main.cpp NotifierBuilderTests.cpp EventTests.cpp)
target_compile_features(inotify_unit_test PRIVATE cxx_std_17)
target_link_libraries(inotify_unit_test
        PRIVATE
          inotify-cpp::inotify-cpp
          CONAN_PKG::boost
          ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME inotify_unit_test COMMAND inotify_unit_test)
