project (InotifyIntegrationTest)

###############################################################################
# Boost
###############################################################################
find_package(Boost 1.61.0 COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable (inotify_integration_test main.cpp)
target_link_libraries (inotify_integration_test LINK_PUBLIC Inotify ${Boost_LIBRARIES})