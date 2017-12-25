ADD_LIBRARY (Inotify
  NotifierBuilder.cpp
  Event.cpp
  FileSystemEvent.cpp
  Inotify.cpp)

TARGET_INCLUDE_DIRECTORIES (Inotify PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
