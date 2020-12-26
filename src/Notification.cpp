#include <inotify-cpp/Notification.h>

namespace inotify {

Notification::Notification(
    const Event& event,
    const std::filesystem::path& path,
    std::chrono::steady_clock::time_point time)
    : event(event)
    , path(path)
    , time(time)
{
}
}