#include <sys/inotify.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include <FileSystemEvent.h>
#include <Inotify.h>

Inotify::Inotify(std::vector<std::string> ignoredFolders,  unsigned eventTimeout, uint32_t eventMask) :
  mError(0),
  mEventTimeout(eventTimeout),
  mLastEventTime(0),
  mEventMask(eventMask),
  mIgnoredFolders(ignoredFolders),
  mIsInitialized(false),
  mInotifyFd(0){
  
  initialize();

}

Inotify::~Inotify(){
  assert(mIsInitialized);
  if(!close(mInotifyFd)){
    mError = errno;
  }
}

bool Inotify::initialize(){
  // Try to initialise inotify
  if(!mIsInitialized){
    mInotifyFd = inotify_init();
    if(mInotifyFd == -1){
      mError = errno;
      return false;
    }
    mIsInitialized = true;
  }
  return true;

}

bool Inotify::watchDirectoryRecursively(boost::filesystem::path path){
  assert(mIsInitialized);
  if(boost::filesystem::exists(path)){
    if(boost::filesystem::is_directory(path)){
      boost::filesystem::recursive_directory_iterator it(path, boost::filesystem::symlink_option::recurse);
      boost::filesystem::recursive_directory_iterator end;
  
      while(it != end){
	boost::filesystem::path currentPath = *it;

	if(boost::filesystem::is_directory(currentPath)){
	  watchFile(currentPath);
	}
	if(boost::filesystem::is_symlink(currentPath)){
	  watchFile(currentPath);
	}
	++it;

      }

    }
    return watchFile(path);
  }
  return false;
}

bool Inotify::watchFile(boost::filesystem::path filePath){
  assert(mIsInitialized);
  mError = 0;
  int wd = 0;
  if(!isIgnored(filePath.string())){
    wd = inotify_add_watch(mInotifyFd, filePath.string().c_str(), mEventMask);
  }

  if(wd == -1){
    mError = errno;
    if(mError == 28){
      std::cout << "Failed to watch" << filePath.string() << "please increase number of watches in /proc/sys/fs/inotify/max_user_watches , Errno:" << mError << std::endl;
      return true;
    }
    return false;

  }
  mFolderMap[wd] = filePath;
  return true;

}

bool Inotify::removeWatch(int wd){
  int result = inotify_rm_watch(mInotifyFd, wd);
  if(result == -1){
    mError = errno;
    return false;
  }
  mFolderMap.erase(wd);
  return true;
}

boost::filesystem::path Inotify::wdToFilename( int wd){
  assert(mIsInitialized);
  return mFolderMap[wd];

}

FileSystemEvent Inotify::getNextEvent(){
  assert(mIsInitialized);
  int length = 0;
  char buffer[EVENT_BUF_LEN];
  time_t currentEventTime = time(NULL);
  std::vector<FileSystemEvent> events;

  // Read Events from fd into buffer
  while(mEventQueue.empty()){
    while(length <= 0 ){
      length = read(mInotifyFd, buffer, EVENT_BUF_LEN);
      currentEventTime = time(NULL);
      if(length == -1){
	mError = errno;
	if(mError != EINTR){
	  continue;

	}

      }

    }

    // Read events from buffer into queue
    currentEventTime = time(NULL);
    int i = 0;
    while(i < length){
      inotify_event *e = ((struct inotify_event*) &buffer[i]);
      boost::filesystem::path path(wdToFilename(e->wd));
      if(!boost::filesystem::is_symlink(path)){
	path = path / std::string(e->name);
      }
      FileSystemEvent fsEvent(e->wd, e->mask, path);
      if(!fsEvent.getPath().empty()){
	events.push_back(fsEvent);
	
      }

      i += EVENT_SIZE + e->len;

    }
    

    // Filter events for timeout
    for(auto eventIter = events.begin(); eventIter < events.end(); ++eventIter){
      if(onTimeout(currentEventTime)){
	events.erase(eventIter);
    
      }
      else{
	mLastEventTime = currentEventTime;
	mEventQueue.push(*eventIter);
      }

    }

  }

  // Return next event
  FileSystemEvent event = mEventQueue.front();
  mEventQueue.pop();
  return event;

}

int Inotify::getLastErrno(){
  return mError;

}

bool Inotify::isIgnored( std::string file){
  if(mIgnoredFolders.empty()){
    return false;
  }

  for(unsigned i = 0; i < mIgnoredFolders.size(); ++i){
    size_t pos = file.find(mIgnoredFolders[i]);
    if(pos!= std::string::npos){
      return true;
    }
  }
  return false;
}

bool Inotify::onTimeout(time_t eventTime){
  return (mLastEventTime + mEventTimeout) > eventTime;
}
