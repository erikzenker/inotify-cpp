/** 
 * @file      Inotify.h
 * @author    Erik Zenker
 * @date      02.11.2012
 * @copyright Gnu Public License
 **/
#pragma once
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

namespace fs = boost::filesystem;

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
 *
 * @eventMask
 *
 * IN_ACCESS         File was accessed (read) (*).
 * IN_ATTRIB         Metadata changed—for example, permissions,
 *                   timestamps, extended attributes, link count
 *                   (since Linux 2.6.25), UID, or GID. (*).
 * IN_CLOSE_WRITE    File opened for writing was closed (*).
 * IN_CLOSE_NOWRITE  File not opened for writing was closed (*).
 * IN_CREATE         File/directory created in watched directory(*).
 * IN_DELETE         File/directory deleted from watched directory(*).
 * IN_DELETE_SELF    Watched file/directory was itself deleted.
 * IN_MODIFY         File was modified (*).
 * IN_MOVE_SELF      Watched file/directory was itself moved.
 * IN_MOVED_FROM     Generated for the directory containing the old
 *                   filename when a file is renamed (*).
 * IN_MOVED_TO       Generated for the directory containing the new
 *                   filename when a file is renamed (*).
 * IN_OPEN           File was opened (*).
 * IN_ALL_EVENTS     macro is defined as a bit mask of all of the above
 *                   events
 * IN_MOVE           IN_MOVED_FROM|IN_MOVED_TO
 * IN_CLOSE          IN_CLOSE_WRITE | IN_CLOSE_NOWRITE
 *
 * See inotify manpage for more event details
 *
 */
class Inotify {
 public:
  Inotify();
  Inotify(uint32_t eventMask);
  Inotify(std::vector< std::string> ignoredDirectories,  unsigned eventTimeout, uint32_t eventMask);
  ~Inotify();
  void watchDirectoryRecursively(fs::path path);
  void watchFile(fs::path file);
  void ignoreFileOnce(fs::path file); // TODO
  FileSystemEvent getNextEvent();
  int getLastErrno();
  
 private:
  fs::path wdToPath(int wd);
  bool isIgnored(std::string file);
  bool onTimeout(time_t eventTime);
  void removeWatch(int wd);  // TODO
  void init();

  // Member
  int mError;
  time_t mEventTimeout;
  time_t mLastEventTime;
  uint32_t mEventMask;
  std::vector<std::string> mIgnoredDirectories;
  std::vector<std::string> mOnceIgnoredDirectories;
  std::queue<FileSystemEvent> mEventQueue;
  std::map<int, fs::path> mDirectorieMap;
  int mInotifyFd;


};

