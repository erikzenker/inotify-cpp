#include <string>
#include <sys/inotify.h>

#include <FileSystemEvent.h>

FileSystemEvent::FileSystemEvent(int wd, uint32_t mask, std::string filename, std::string watchFolder) :
  mWd(wd),
  mMask(mask),
  mFilename(filename){
  
  if(watchFolder[watchFolder.size()-1] != '/')
    watchFolder.append("/");
  mWatchFolder = watchFolder;
}

FileSystemEvent::~FileSystemEvent(){

}

uint32_t FileSystemEvent::getMask(){
  return mMask;
}

std::string FileSystemEvent::getFilename(){
  return mFilename;
}

int FileSystemEvent::getWd(){
  return mWd;
}

std::string FileSystemEvent::getWatchFolder(){
  return mWatchFolder;
}

std::string FileSystemEvent::getMaskString(){
  return maskToString(mMask);
}

std::string FileSystemEvent::getFullPath(){
  return mWatchFolder.append(mFilename);
}

std::string FileSystemEvent::maskToString(uint32_t mask){
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
