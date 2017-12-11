project (InotifyExample)

###############################################################################
# Boost
###############################################################################
find_package(Boost 1.61.0 COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable (inotify_example main.cpp)
target_link_libraries (inotify_example LINK_PUBLIC Inotify ${Boost_LIBRARIES})