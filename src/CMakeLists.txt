set(LIB_NAME inotify-cpp)
set(LIB_COMPANY inotify-cpp)
set(LIB_SRCS NotifierBuilder.cpp Event.cpp FileSystemEvent.cpp Inotify.cpp Notification.cpp)
set(LIB_HEADER
        include/inotify-cpp/NotifierBuilder.h
        include/inotify-cpp/Event.h
        include/inotify-cpp/FileSystemEvent.h
        include/inotify-cpp/Inotify.h
        include/inotify-cpp/Notification.h)

cmake_minimum_required(VERSION 3.5)
project(${LIB_NAME} VERSION 0.0.1)

# dependencies
find_package(
        Boost 1.54.0
        COMPONENTS system filesystem
        REQUIRED
)

# library definition
add_library(${LIB_NAME} SHARED ${LIB_SRCS} ${LIB_HEADER})

set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS ${LIB_NAME} EXPORT ${LIB_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(EXPORT ${LIB_NAME}Targets
        DESTINATION lib/cmake/${LIB_NAME}
        FILE ${LIB_NAME}Targets.cmake
        NAMESPACE ${LIB_COMPANY}::)
install(DIRECTORY include/${LIB_NAME}
        DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${LIB_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion)
install(FILES ${LIB_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake
        DESTINATION lib/cmake/${LIB_NAME})

target_compile_features(${LIB_NAME} PUBLIC cxx_std_11)
target_include_directories(${LIB_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(${LIB_NAME}
        PRIVATE
          Boost::system
          Boost::filesystem)
add_library(${LIB_COMPANY}::${LIB_NAME} ALIAS ${LIB_NAME})