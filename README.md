Inotify-cpp [![Build Status](https://travis-ci.org/erikzenker/inotify-cpp.svg?branch=master)](https://travis-ci.org/erikzenker/inotify-cpp) [![Coverity Scan Build Status](https://scan.coverity.com/projects/14692/badge.svg)](https://scan.coverity.com/projects/erikzenker-inotify-cpp)
===========

__Inotify-cpp__ is a C++ wrapper for the linux inotify tool. It lets you watch for 
filesystem events on your filesystem tree. 

## Usage ##
 
  ```c++
  
#include <inotify/NotifierBuilder.h>

#include <boost/filesystem.hpp>

#include <iostream>

using namespace inofity;

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cout << "Usage: ./inotify_example /path/to/dir" << std::endl;
    exit(0);
  }

  // Directory to watch
  boost::filesystem::path dir(argv[1]);

  auto handleNotification = [&](Notification notification) {
    std::cout << "Event" << notification.event << " on " <<  notification.path << " was triggered." << std::endl;
  };

  std::cout << "Setup watches for " << dir << "..." << std::endl;
  auto notifier = BuildNotifier()
    .watchPathRecursively(dir)
    .ignoreFileOnce("file")
    .onEvents({Event::create, Event::modify, Event::remove, Event::move}, handleNotification);

  std::cout << "Waiting for events..." << std::endl;
  notifier.run();

  return 0;
}

  ``` 

## Build Example ##
```bash
mkdir build; cd build
cmake ..
cmake --build .
./example/inotify_example
```

## Dependencies ##
 + boost 1.61.1
 + c++11
 + linux 2.6.13

## Licence
MIT

## Author ##
Written by Erik Zenker (erikzenker@hotmail.com)
