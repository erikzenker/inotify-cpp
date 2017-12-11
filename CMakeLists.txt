cmake_minimum_required(VERSION 3.2.2)
project (Inotify)

set (CMAKE_CXX_STANDARD 11)

add_subdirectory (test)
add_subdirectory (example)

add_library (Inotify INTERFACE)
target_include_directories (Inotify INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
