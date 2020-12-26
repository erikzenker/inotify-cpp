#pragma once

#include <inotify-cpp/Event.h>

#include <filesystem>

#include <chrono>

namespace inotify {

class Notification {
  public:
    Notification(
        const Event& event,
        const std::filesystem::path& path,
        std::chrono::steady_clock::time_point time);

  public:
    const Event event;
    const std::filesystem::path path;
    const std::chrono::steady_clock::time_point time;
};
}