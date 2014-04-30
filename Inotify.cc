#include <sys/inotify.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <boost/filesystem.hpp>


#include <FileSystemEvent.h>
#include <Inotify.h>

Inotify::Inotify() :
  mError(0),
  mEventTimeout(0),
  mLastEventTime(0),
  mEventMask(IN_ALL_EVENTS),
  mIgnoredDirectories(std::vector<std::string>()),
  mInotifyFd(0){

  // Initialize inotify
  init();
}

Inotify::Inotify(uint32_t eventMask) :
  mError(0),
  mEventTimeout(0),
  mLastEventTime(0),
  mEventMask(eventMask),
  mIgnoredDirectories(std::vector<std::string>()),
  mInotifyFd(0){

  // Initialize inotify
  init();
}

Inotify::Inotify(std::vector<std::string> ignoredDirectories,  unsigned eventTimeout, uint32_t eventMask) :
  mError(0),
  mEventTimeout(eventTimeout),
  mLastEventTime(0),
  mEventMask(eventMask),
  mIgnoredDirectories(ignoredDirectories),
  mInotifyFd(0){
  
  // Initialize inotify
  init();
}

Inotify::~Inotify(){
  if(!close(mInotifyFd)){
    mError = errno;
  }

}

void Inotify::init(){
  mInotifyFd = inotify_init();
  if(mInotifyFd == -1){
    mError = errno;
    std::stringstream errorStream;
    errorStream << "Can't initialize inotify ! " << strerror(mError) << ".";
    throw std::runtime_error(errorStream.str());
  }

}

/**
 * @brief Adds the given path and all files and subdirectories
 *        to the set of watched files/directories.
 *        Symlinks will be followed!
 *
 * @param path that will be watched recursively
 *
 */
void Inotify::watchDirectoryRecursively(fs::path path){
  if(fs::exists(path)){
    if(fs::is_directory(path)){
      fs::recursive_directory_iterator it(path, fs::symlink_option::recurse);
      fs::recursive_directory_iterator end;
  
      while(it != end){
	fs::path currentPath = *it;

	if(fs::is_directory(currentPath)){
	  watchFile(currentPath);
	}
	if(fs::is_symlink(currentPath)){
	  watchFile(currentPath);
	}
	++it;

      }

    }
    watchFile(path);
  }
  else {
    throw std::invalid_argument("Can´t watch Path! Path does not exist. Path: " + path.string());
  }

}

/**
 * @brief Adds a single file/directorie to the list of
 *        watches. Path and corresponding watchdescriptor
 *        will be stored in the directorieMap. This is done
 *        because events on watches just return this
 *        watchdescriptor.
 *
 * @param path that will be watched
 *
 */
void Inotify::watchFile(fs::path filePath){
  if(fs::exists(filePath)){
    mError = 0;
    int wd = 0;
    if(!isIgnored(filePath.string())){
      wd = inotify_add_watch(mInotifyFd, filePath.string().c_str(), mEventMask);
    }

    if(wd == -1){
      mError = errno;
      std::stringstream errorStream;
      if(mError == 28){
	errorStream << "Failed to watch! " << strerror(mError) << ". Please increase number of watches in \"/proc/sys/fs/inotify/max_user_watches\".";
	throw std::runtime_error(errorStream.str());
      }

      errorStream << "Failed to watch! " << strerror(mError) << ". Path: " << filePath.string();
      throw std::runtime_error(errorStream.str());

    }
    mDirectorieMap[wd] = filePath;
  }

}


void Inotify::ignoreFileOnce(fs::path file){
  mOnceIgnoredDirectories.push_back(file.string());

}

/**
 * @brief Removes watch from set of watches. This
 *        is not done recursively!
 *
 * @param wd watchdescriptor
 *
 */
void Inotify::removeWatch(int wd){
  int result = inotify_rm_watch(mInotifyFd, wd);
  if(result == -1){
    mError = errno;
    std::stringstream errorStream;
    errorStream << "Failed to remove watch! " << strerror(mError) << ".";
    throw std::runtime_error(errorStream.str());
  }
  mDirectorieMap.erase(wd);
}


fs::path Inotify::wdToPath(int wd){
  return mDirectorieMap[wd];

}

/**
 * @brief Blocking wait on new events of watched files/directories 
 *        specified on the eventmask. FileSystemEvents
 *        will be returned one by one. Thus this
 *        function can be called in some while(true)
 *        loop.
 *
 * @return A new FileSystemEvent
 *
 */
FileSystemEvent Inotify::getNextEvent(){
  int length = 0;
  char buffer[EVENT_BUF_LEN];
  time_t currentEventTime = time(NULL);
  std::vector<FileSystemEvent> events;

  // Read Events from fd into buffer
  while(mEventQueue.empty()){
    length = 0;
    memset(&buffer, 0, EVENT_BUF_LEN);
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
      inotify_event *event = ((struct inotify_event*) &buffer[i]);
      fs::path path(wdToPath(event->wd) / std::string(event->name));
      if(fs::is_directory(path)){
	event->mask |= IN_ISDIR;
      }
      FileSystemEvent fsEvent(event->wd, event->mask, path);

      if(!fsEvent.path.empty()){
	events.push_back(fsEvent);
	
      }
      else{
	// Event is not complete --> ignore
      }

      i += EVENT_SIZE + event->len;

    }
    

    // Filter events
    for(auto eventIt = events.begin(); eventIt < events.end(); ++eventIt){
      FileSystemEvent currentEvent = *eventIt;
      if(onTimeout(currentEventTime)){
	events.erase(eventIt);
    
      }
      else if(isIgnored(currentEvent.path.string())){
      	events.erase(eventIt);
      }
      else{
	mLastEventTime = currentEventTime;
	mEventQueue.push(currentEvent);
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

bool Inotify::isIgnored(std::string file){
  if(mIgnoredDirectories.empty() and mOnceIgnoredDirectories.empty()){
    return false;
  }

  for(unsigned i = 0; i < mOnceIgnoredDirectories.size(); ++i){
    size_t pos = file.find(mOnceIgnoredDirectories[i]);
    if(pos!= std::string::npos){
      mOnceIgnoredDirectories.erase(mOnceIgnoredDirectories.begin() + i);
      return true;
    }
  }

  for(unsigned i = 0; i < mIgnoredDirectories.size(); ++i){
    size_t pos = file.find(mIgnoredDirectories[i]);
    if(pos!= std::string::npos){
      return true;
    }
  }

  return false;
}

bool Inotify::onTimeout(time_t eventTime){
  return (mLastEventTime + mEventTimeout) > eventTime;
}
