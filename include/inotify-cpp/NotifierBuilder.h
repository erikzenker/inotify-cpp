#pragma once

#include <inotify/Inotify.h>
#include <inotify/Notification.h>

#include <boost/filesystem.hpp>

#include <memory>
#include <string>

namespace inotify {

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
  public:
    NotifierBuilder();

    auto run() -> void;
    auto runOnce() -> bool;
    auto stop() -> void;
    auto watchPathRecursively(boost::filesystem::path path) -> NotifierBuilder&;
    auto watchFile(boost::filesystem::path file) -> NotifierBuilder&;
    auto ignoreFileOnce(boost::filesystem::path file) -> NotifierBuilder&;
    auto onEvent(Event event, EventObserver) -> NotifierBuilder&;
    auto onEvents(std::vector<Event> event, EventObserver) -> NotifierBuilder&;
    auto onUnexpectedEvent(EventObserver) -> NotifierBuilder&;

  private:
    std::shared_ptr<Inotify> mInotify;
    std::map<Event, EventObserver> mEventObserver;
    EventObserver mUnexpectedEventObserver;
};

NotifierBuilder BuildNotifier();
}