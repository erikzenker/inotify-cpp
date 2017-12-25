#pragma once
#include <boost/filesystem.hpp>

#include <string>

namespace inotify {
class FileSystemEvent {
  public:
    FileSystemEvent(int wd, uint32_t mask, const boost::filesystem::path path);

    ~FileSystemEvent();

  public: // Member
    int wd;
    uint32_t mask;
    boost::filesystem::path path;
};
}