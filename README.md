Inotify 
=======
A C++ header only library which wraps inotify which is threadsafe

## Description ##
 __Inotify__ is a C++ class, that lets you use linux inotify to watch files or directories.
See man inotify for more background information.

## Build ##
   + Include Inotify.h FileSystemEvent.h
   + compile with -lboost_system -lboost_filesystem -std=c++11
   + example: clang++ main.cc -I . -std=c++11 -lboost_system -lboost_filesystem
     or just: make

## Example ##
Simple example for Inotify usage (see main.cc).
```c++

#include <Inotify.h>
#include <FileSystemEvent.h>
#include <boost/filesystem.hpp>

int main(int argc, char** argv){
  if(argc <= 1){
    std::cout << "Usage: ./a.out /path/to/path/dir" << std::endl;
    exit(0);
  }
  // Directory to watch
  boost::filesystem::path dir(argv[1]);

  // Init inotify
  std::cout << "Setup watches for " << dir <<"..." << std::endl;
  Inotify inotify(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE);
  
  // Watch a directory (plus all subdirectories and files)
  inotify.watchDirectoryRecursively(dir);

  inotify.ignoreFileOnce("file");
  
  // Wait for event of this directory
  std::cout << "Waiting for events..." << std::endl;
  while(true){
    FileSystemEvent event = inotify.getNextEvent();
    std::cout << "Event wd(" << event.wd << ") " << event.getMaskString() << "for " << event.path << " was triggered!" << std::endl;
  }
  
  return 0;
}


```
   
## Dependencies ##
 + boost
 + c++11
 + linux 2.6.13

## Copyright
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.  
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.

## Author ##
Written by Erik Zenker (erikzenker@hotmail.com)
