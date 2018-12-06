include(CMakeFindDependencyMacro)
find_dependency(Boost 1.54.0)
include("${CMAKE_CURRENT_LIST_DIR}/inotify-cppTargets.cmake")