#pragma once
#include <inotify-cpp/FileSystemAdapter.h>

#include <chrono>
#include <string>

namespace inotify {
class FileSystemEvent {
  public:
    FileSystemEvent(
        int wd,
        uint32_t mask,
        const inotifypp::filesystem::path& path,
        const std::chrono::steady_clock::time_point& eventTime);

    ~FileSystemEvent();

  public:
    int wd;
    uint32_t mask;
    inotifypp::filesystem::path path;
    std::chrono::steady_clock::time_point eventTime;
};
}
