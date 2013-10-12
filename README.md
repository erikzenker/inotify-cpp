Inotify 
=======
A C++ interface for linux inotify which is threadsafe

## Description ##
 __Inotify__ is a C++ class, that lets you use linux inotify to watch files or directories.
See man inotify for more background information.

## Build ##
   + Include Inotify.h FileSystemEvent.h
   + compile with -lboost_system -lboost_filesystem -std=c++11
   + example: g++ main.cc Inotify.cc FileSystemEvent.cc -I . -std=c++11 -lboost_system -lboost_filesystem

## Example ##
Simple example for Inotify usage.
```c++

#include <Inotify.h>
#include <FileSystemEvent.h>

int main(int argc, char** argv){
  if(argc <= 1){
    std::cout << "Usage: ./a.out /path/to/path/dir" << std::endl;
    exit(0);
  }

  // Directory to watch
  std::string dir(argv[1]);

  // Init constructor arguments
  std::vector<std::string> ignoredDirectories;
  unsigned eventTimeout = 0;
  uint32_t eventMask = IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE;

  // Init inotify
  Inotify inotify(ignoredDirectories, eventTimeout, eventMask);
  
  // Watch a directory (plus all subdirectories and files)
  if(!inotify.watchDirectoryRecursively(dir)){
    std::cout << "Can´t watch directory " << dir << "! " << strerror(inotify.getLastErrno()) << "!"<<std::endl;
    exit(0);
  }
  
  // Wait for event of this directory
  while(true){
    FileSystemEvent event = inotify.getNextEvent();
    std::cout << "Event wd(" << event.getWd() << ") " << event.getMaskString() << "for " << event.getPath() << " was triggered!" << std::endl;
  }
  
  return 0;
}

```
   
## Dependencies ##
 + boost
 + c++11
 + linux 2.6.13

## Copyrigth
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.  
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.

## Author ##
Written by Erik Zenker (erikzenker@hotmail.com)
