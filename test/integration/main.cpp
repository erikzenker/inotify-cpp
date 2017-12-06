#include <inotify/Inotify.h>
#include <inotify/FileSystemEvent.h>
#include <inotify/NotifierBuilder.h>

#include <boost/filesystem.hpp>

#include <iostream>



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
    std::cout << "Event wd(" << event.wd << ") " << event.mask << "for " << event.path << " was triggered!" << std::endl;
  }

  return 0;
}
