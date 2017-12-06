Inotify-cpp
=======

__Inotify-cpp__ is a C++ wrapper for the linux inotify tool. It lets you watch for 
filesystem events on your filesystem tree. 

## Usage ##
 
  ```c++
  
  #include <inotify/Inotify.h>
  #include <inotify/FileSystemEvent.h>
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

## Build Example ##
```bash
mkdir build; cd build
cmake ..
./test/inotify_test
```

## Dependencies ##
 + boost 1.61.1
 + c++11
 + linux 2.6.13

## Licence
MIT

## Author ##
Written by Erik Zenker (erikzenker@hotmail.com)
