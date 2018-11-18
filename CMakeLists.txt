cmake_minimum_required(VERSION 3.5)
project(inotify-cpp)

add_subdirectory(src)

add_subdirectory(example)

enable_testing()
add_subdirectory(test)

