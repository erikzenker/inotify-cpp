project (InotifyExample)

###############################################################################
# Boost
###############################################################################
find_package(Boost 1.54.0 COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable (inotify_example main.cpp ../source/CMakeLists.txt)
target_link_libraries (inotify_example PUBLIC Inotify ${Boost_LIBRARIES})