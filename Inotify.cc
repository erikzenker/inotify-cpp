#include <sys/inotify.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

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

bool Inotify::watchDirectoryRecursively( std::string watchPath){
  boost::filesystem::path path(watchPath);
  if(boost::filesystem::is_directory(watchPath)){
      boost::filesystem::recursive_directory_iterator it(path);
      boost::filesystem::recursive_directory_iterator end;
  
      while(it != end){

	if(boost::filesystem::is_regular_file(*it)){
	  watchFile(*it);
	}
	if(boost::filesystem::is_directory(*it)){
	  watchFile(*it);
	}

	++it;

      }

    }
  return watchFile(path);
}

bool Inotify::watchFile(boost::filesystem::path file){
  assert(mIsInitialized);
  mError = 0;
  int wd = 0;
  if(!isIgnored(file.string())){
    wd = inotify_add_watch(mInotifyFd, file.string().c_str(), mEventMask);
  }
  else{
    return true;
  }

  if(wd == -1){
    mError = errno;
    if(mError == 28){
      std::cout << "Failed to watch" << file.string() << "please increase number of watches in /proc/sys/fs/inotify/max_user_watches , Errno:" << mError << std::endl;
      return true;
    }
    return false;

  }
  mFolderMap[wd] = file.string();
  return true;

}

bool Inotify::removeWatch( int wd){
  int error = inotify_rm_watch(mInotifyFd, wd);
  if(errno <= 0){
    return false;
  }
  mFolderMap.erase(wd);
  return true;
}

std::string Inotify::wdToFilename( int wd){
  assert(mIsInitialized);
  return mFolderMap[wd];

}

std::string Inotify::maskToString( uint32_t mask){
   std::string maskString = "";

  if(IN_ACCESS & mask)
    maskString.append("IN_ACCESS ");
  if(IN_ATTRIB & mask)
    maskString.append("IN_ATTRIB ");
  if(IN_CLOSE_WRITE & mask)
    maskString.append("IN_CLOSE_WRITE ");
  if(IN_CLOSE_NOWRITE & mask)
    maskString.append("IN_CLOSE_NOWRITE ");
  if(IN_CREATE & mask)
    maskString.append("IN_CREATE ");
  if(IN_DELETE & mask)
    maskString.append("IN_DELETE ");
  if(IN_DELETE_SELF & mask)
    maskString.append("IN_DELETE_SELF ");
  if(IN_MODIFY & mask)
    maskString.append("IN_MODIFY ");
  if(IN_MOVE_SELF & mask)
    maskString.append("IN_MOVE_SELF ");
  if(IN_MOVED_FROM & mask)
    maskString.append("IN_MOVED_FROM ");
  if(IN_MOVED_TO & mask)
    maskString.append("IN_MOVED_TO ");
  if(IN_OPEN & mask)
    maskString.append("IN_OPEN ");
  if(IN_ISDIR & mask)
    maskString.append("IN_ISDIR ");
  if(IN_UNMOUNT & mask)
    maskString.append("IN_UNMOUNT ");
  if(IN_Q_OVERFLOW & mask)
    maskString.append("IN_Q_OVERFLOW ");
  if(IN_CLOSE & mask)
    maskString.append("IN_CLOSE ");
  if(IN_IGNORED & mask)
    maskString.append("IN_IGNORED ");
  if(IN_ONESHOT & mask)
    maskString.append("IN_ONESHOT ");

  return maskString;
}

inotify_event Inotify::getNextEvent(){
  assert(mIsInitialized);
  inotify_event event;
  int length = 0;
  char buffer[EVENT_BUF_LEN];
  time_t currentEventTime = time(NULL);
  std::vector<inotify_event> events;

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
      event = *((struct inotify_event*) &buffer[i]);
      //if(checkEvent(event) && event.mask != IN_IGNORED){
      if(checkEvent(event)){
	events.push_back(event);

      }
      i += EVENT_SIZE + event.len;

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
  event = mEventQueue.front();
  mEventQueue.pop();

  return event;

}

int Inotify::getLastError(){
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

void Inotify::clearEventQueue(){
  for(unsigned eventCount = 0; eventCount < mEventQueue.size(); eventCount++){
    mEventQueue.pop();
  }

}

bool Inotify::onTimeout( time_t eventTime){
  return (mLastEventTime + mEventTimeout) > eventTime;

}

bool Inotify::checkEvent( inotify_event event){
  // Events seems to have no syncfolder,
  // this can happen if not the full event
  // was read from inotify fd
  if(!wdToFilename(event.wd).compare("")){
    return false;
  }

  return true;

}
