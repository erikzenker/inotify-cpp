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
  std::cout << "Setup watches for \"" << dir.string() <<"\"..." << std::endl;
  Inotify inotify(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE);
  
  // Watch a directory (plus all subdirectories and files)
  inotify.watchDirectoryRecursively(dir);

  inotify.ignoreFileOnce("file");
  
  // Wait for event of this directory
  std::cout << "Waiting for events..." << std::endl;
  while(true){
    FileSystemEvent event = inotify.getNextEvent();
    std::cout << "Event wd(" << event.getWd() << ") " << event.getMaskString() << "for " << event.getPath() << " was triggered!" << std::endl;
  }
  
  return 0;
}
