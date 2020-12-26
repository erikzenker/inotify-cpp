set(LIB_NAME inotify-cpp)
set(LIB_COMPANY inotify-cpp)
set(LIB_SRCS NotifierBuilder.cpp Event.cpp FileSystemEvent.cpp Inotify.cpp Notification.cpp)
set(LIB_HEADER
        include/inotify-cpp/NotifierBuilder.h
        include/inotify-cpp/Event.h
        include/inotify-cpp/FileSystemEvent.h
        include/inotify-cpp/Inotify.h
        include/inotify-cpp/Notification.h)

cmake_minimum_required(VERSION 3.8)
project(${LIB_NAME} VERSION 0.2.0)
include(GNUInstallDirs)

# dependencies
find_package(
        Boost 1.54.0
        REQUIRED
)

macro(configure_target target)
        set_target_properties(${target} PROPERTIES
                LINKER_LANGUAGE CXX
                OUTPUT_NAME ${LIB_NAME}
                SOVERSION ${PROJECT_VERSION}
        )

        install(TARGETS ${target} EXPORT ${target}Targets
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
        install(EXPORT ${target}Targets
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
                FILE ${target}Targets.cmake
                NAMESPACE ${LIB_COMPANY}::)

        target_compile_features(${target} PUBLIC cxx_std_11)
        target_include_directories(${target} PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>)
        target_compile_features(${target} PRIVATE cxx_std_17)
endmacro(configure_target target)

# library definition
if(BUILD_SHARED_LIBS)
        add_library(${LIB_NAME}-shared SHARED ${LIB_SRCS} ${LIB_HEADER})
        configure_target(${LIB_NAME}-shared)
        if(NOT BUILD_STATIC_LIBS)
                add_library(${LIB_COMPANY}::${LIB_NAME} ALIAS ${LIB_NAME}-shared)
        endif()
endif()

if(BUILD_STATIC_LIBS)
        add_library(${LIB_NAME}-static STATIC ${LIB_SRCS} ${LIB_HEADER})
        configure_target(${LIB_NAME}-static)
        if(NOT BUILD_SHARED_LIBS)
                add_library(${LIB_COMPANY}::${LIB_NAME} ALIAS ${LIB_NAME}-static)
        endif()
endif()

if(BUILD_SHARED_LIBS AND BUILD_STATIC_LIBS)
        add_library(${LIB_COMPANY}::${LIB_NAME} ALIAS ${LIB_NAME}-shared)
endif()

install(DIRECTORY ${CMAKE_INSTALL_INCLUDEDIR}/${LIB_NAME} DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${LIB_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion)
install(FILES ${LIB_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME})
