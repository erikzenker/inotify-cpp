/**
 * @file      FileSystemEvent.h
 * @author    Erik Zenker
 * @date      01.11.2012
 * @copyright Gnu Public License
 **/

#ifndef FileSystemEvent_H
#define FileSystemEvent_H

#include <string>
#include <boost/filesystem.hpp>

/**
 * @brief Container for events on filesystem.
 * @class FileSystemEvent
 *        FileSystemEvent.h
 *        "include/FileSystemEvent.h"
 *
 * The intention for FileSystemEvent was to
 * create some common interface for different
 * filesystem scan methods. For example you
 * could poll your filesystem every 10 seconds
 * to seek for changes or use inotify which detects 
 * events by it self.
 *
 **/
class FileSystemEvent {
 public:
  FileSystemEvent(int wd, uint32_t mask, boost::filesystem::path path);
  ~FileSystemEvent();
  uint32_t getMask();
  int getWd();
  std::string getMaskString();
  boost::filesystem::path getPath();

 private:
  std::string maskToString(uint32_t events);

  // Member
  int mWd;
  uint32_t mMask;
  boost::filesystem::path mPath;

};

#endif /* FileSystemEvent_H */
