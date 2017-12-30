cmake_minimum_required(VERSION 3.2.2)
project(InotifyExample)

###############################################################################
# Boost
###############################################################################
find_package(
  Boost 1.54.0
  COMPONENTS system filesystem unit_test_framework
  REQUIRED
)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_library(
  INOTIFY_CPP_LIBRARY inotify-cpp
  HINTS ${CMAKE_SOURCE_DIR}/../source
)

add_executable(inotify_example main.cpp)
target_link_libraries(
  inotify_example
  PUBLIC ${INOTIFY_CPP_LIBRARY} ${Boost_LIBRARIES}
)
target_include_directories(
  inotify_example
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
