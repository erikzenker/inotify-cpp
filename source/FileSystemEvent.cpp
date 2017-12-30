#include <inotify-cpp/FileSystemEvent.h>

#include <sys/inotify.h>

namespace inotify {
FileSystemEvent::FileSystemEvent(const int wd, uint32_t mask, const boost::filesystem::path path)
    : wd(wd)
    , mask(mask)
    , path(path)
{
}

FileSystemEvent::~FileSystemEvent()
{
}
}