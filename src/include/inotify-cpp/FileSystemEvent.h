#pragma once
#include <boost/filesystem.hpp>

#include <chrono>
#include <string>

namespace inotify {
class FileSystemEvent {
  public:
    FileSystemEvent(
        int wd,
        uint32_t mask,
        const boost::filesystem::path& path,
        const std::chrono::steady_clock::time_point& eventTime);

    ~FileSystemEvent();

  public:
    int wd;
    uint32_t mask;
    boost::filesystem::path path;
    std::chrono::steady_clock::time_point eventTime;
};
}