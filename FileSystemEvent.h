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
  FileSystemEvent(int wd, uint32_t mask, const boost::filesystem::path path);
  ~FileSystemEvent();
  std::string getMaskString() const;

  /* Member */
  bool isRecursive;
  int wd;
  uint32_t mask;
  boost::filesystem::path path;

 private:
  std::string maskToString(uint32_t events) const;

};

