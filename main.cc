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
