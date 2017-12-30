project(InotifyUnitTest)

###############################################################################
# Thread
###############################################################################
find_package(Threads)

add_executable(inotify_unit_test main.cpp NotifierBuilderTests.cpp)
target_link_libraries(
  inotify_unit_test
  PUBLIC inotify-cpp ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME inotify_unit_test COMMAND inotify_unit_test)
