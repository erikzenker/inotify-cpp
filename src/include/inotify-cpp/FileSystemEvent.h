#pragma once
#include <filesystem>

#include <chrono>
#include <string>

namespace inotify {
class FileSystemEvent {
  public:
    FileSystemEvent(
        int wd,
        uint32_t mask,
        const std::filesystem::path& path,
        const std::chrono::steady_clock::time_point& eventTime);

    ~FileSystemEvent();

  public:
    int wd;
    uint32_t mask;
    std::filesystem::path path;
    std::chrono::steady_clock::time_point eventTime;
};
}