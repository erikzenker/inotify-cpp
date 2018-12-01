# Inotify-cpp #

[![Build Status](https://travis-ci.org/erikzenker/inotify-cpp.svg?branch=master)](https://travis-ci.org/erikzenker/inotify-cpp) [![Coverity Scan Build Status](https://scan.coverity.com/projects/14692/badge.svg)](https://scan.coverity.com/projects/erikzenker-inotify-cpp) [![codecov](https://codecov.io/gh/erikzenker/inotify-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/erikzenker/inotify-cpp) [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/erikzenker)
===========

__Inotify-cpp__ is a C++ wrapper for linux inotify. It lets you watch for
filesystem events on your filesystem tree. The following usage example shows
the implementation of a simple filesystem event watcher for the commandline.

## Usage ##

  ```c++
#include <inotify-cpp/NotifierBuilder.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace inotify;

int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::cout << "Usage: ./inotify_example /path/to/dir" << std::endl;
        exit(0);
    }

    // Parse the directory to watch
    boost::filesystem::path path(argv[1]);

    // Set the event handler which will be used to process particular events
    auto handleNotification = [&](Notification notification) {
        std::cout << "Event " << notification.event << " on " << notification.path << " at "
                  << notification.time.time_since_epoch().count() << " was triggered." << std::endl;
    };

    // Set the a separate unexpected event handler for all other events. An exception is thrown by
    // default.
    auto handleUnexpectedNotification = [](Notification notification) {
        std::cout << "Event " << notification.event << " on " << notification.path << " at "
                  << notification.time.time_since_epoch().count()
                  << " was triggered, but was not expected" << std::endl;
    };

    // Set the events to be notified for
    auto events = { Event::open | Event::is_dir, // some events occur in combinations
                    Event::access,
                    Event::create,
                    Event::modify,
                    Event::remove,
                    Event::move };

    // The notifier is configured to watch the parsed path for the defined events. Particular files
    // or paths can be ignored(once).
    auto notifier = BuildNotifier()
                        .watchPathRecursively(path)
                        .ignoreFileOnce("fileIgnoredOnce")
                        .ignoreFile("fileIgnored")
                        .onEvents(events, handleNotification)
                        .onUnexpectedEvent(handleUnexpectedNotification);

    // The event loop is started in a separate thread context.
    std::thread thread([&](){ notifier.run(); });

    // Terminate the event loop after 60 seconds
    std::this_thread::sleep_for(std::chrono::seconds(60));
    notifier.stop();
    thread.join();
    return 0;
}
  ```
  
## Build Library ##
```bash
mkdir build; cd bulid
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
cmake --build .

# run tests
ctest -VV

# install the library
make install
```

## Build Example ##
Build and install the library before you run the following commands:
```bash
mkdir build; cd build
cmake ../example
cmake --build inotify_example
./inotify_example
```

## Install from Packet ##
* Arch Linux: `yaourt -S inotify-cpp-git`

## Dependencies ##
 + lib
   + boost 1.54.0
   + c++11
   + linux 2.6.13
 + build
   + cmake 3.5   

## Licence
MIT

## Author ##
Written by Erik Zenker (erikzenker@hotmail.com)

## Thanks for Contribution ##
  + [spelcaster](https://github.com/spelcaster)
