/** 
 * @file      Inotify.h
 * @author    Erik Zenker
 * @date      02.11.2012
 * @copyright Gnu Public License
 **/
#ifndef INOTIFY_H
#define INOTIFY_H

#include <sys/inotify.h>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>

#include <FileSystemEvent.h>

#define MAX_EVENTS     4096
#define EVENT_SIZE     (sizeof (inotify_event))
#define EVENT_BUF_LEN  (MAX_EVENTS * (EVENT_SIZE + 16))

/**
 * @brief C++ wrapper for linux inotify interface
 * @class Inotify
 *        Inotify.h
 *        "include/Inotify.h"
 *
 * folders will be watched by watchFolderRecursively or
 * files by watchFile. If there are changes inside this
 * folder or files events will be raised. This events
 * can be get by getNextEvent.
 **/
class Inotify {
 public:
  Inotify(std::vector< std::string> ignoredFolders,  unsigned eventTimeout, uint32_t eventMask);
  ~Inotify();
  bool watchDirectoryRecursively(boost::filesystem::path path);
  bool watchFile(boost::filesystem::path file);
  bool removeWatch(int wd);
  FileSystemEvent getNextEvent();
  int getLastErrno();
  boost::filesystem::path wdToFilename(int wd);
  std::string maskToString(uint32_t mask);
  
 private:
  bool initialize();
  bool isIgnored(std::string file);
  void clearEventQueue();
  bool onTimeout(time_t eventTime);
  bool checkEvent(FileSystemEvent event);

  // Member
  int mError;
  time_t mEventTimeout;
  time_t mLastEventTime;
  uint32_t mEventMask;
  std::vector<std::string> mIgnoredFolders;
  std::queue<FileSystemEvent> mEventQueue;
  std::map<int, boost::filesystem::path> mFolderMap;
  bool mIsInitialized;
  int mInotifyFd;


};

#endif /* INOTIFY_H */
