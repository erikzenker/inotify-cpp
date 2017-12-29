#pragma once

#include <inotify/Event.h>

#include <boost/filesystem.hpp>

namespace inotify {

struct Notification {
    Event event;
    boost::filesystem::path path;
};
}