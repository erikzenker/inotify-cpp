/**
 * @file      FileSystemEvent.h
 * @author    Erik Zenker
 * @date      01.11.2012
 * @copyright Gnu Public License
 **/

#pragma once
#include <string>
#include <boost/filesystem.hpp>
#include <sys/inotify.h>

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


inline FileSystemEvent::FileSystemEvent(const int wd, uint32_t mask, const boost::filesystem::path path) :
  isRecursive(false),
  wd(wd),
  mask(mask),
  path(path){

}

inline FileSystemEvent::~FileSystemEvent(){
}

inline std::string FileSystemEvent::getMaskString() const{
  return maskToString(mask);
}

inline std::string FileSystemEvent::maskToString(uint32_t mask) const{
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

