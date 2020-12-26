#include <inotify-cpp/FileSystemEvent.h>

#include <sys/inotify.h>

namespace inotify {
FileSystemEvent::FileSystemEvent(
    const int wd,
    uint32_t mask,
    const std::filesystem::path& path,
    const std::chrono::steady_clock::time_point& eventTime)
    : wd(wd)
    , mask(mask)
    , path(path)
    , eventTime(eventTime)
{
}

FileSystemEvent::~FileSystemEvent()
{
}
}