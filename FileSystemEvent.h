/**
 * @file      FileSystemEvent.h
 * @author    Erik Zenker
 * @date      01.11.2012
 * @copyright Gnu Public License
 **/

#pragma once
#include <string>
#include <boost/filesystem.hpp>

/**
 * @brief Container for events on filesystem.
 * @class FileSystemEvent
 *        FileSystemEvent.h
 *        "include/FileSystemEvent.h"
 *
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

