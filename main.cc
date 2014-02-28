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
  std::cout << "Setup watches for \"" << dir <<"\"..." << std::endl;
  Inotify inotify(ignoredDirectories, eventTimeout, eventMask);
  
  // Watch a directory (plus all subdirectories and files)
  inotify.watchDirectoryRecursively(dir);
  
  // Wait for event of this directory
  std::cout << "Waiting for events..." << std::endl;
  while(true){
    FileSystemEvent event = inotify.getNextEvent();
    std::cout << "Event wd(" << event.getWd() << ") " << event.getMaskString() << "for " << event.getPath() << " was triggered!" << std::endl;
  }
  
  return 0;
}
