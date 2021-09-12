#pragma once

#include <inotify-cpp/Inotify.h>
#include <inotify-cpp/Notification.h>
#include <inotify-cpp/FileSystemAdapter.h>

#include <memory>
#include <string>

namespace inotify {

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
  public:
    NotifierBuilder();

    auto run() -> void;
    auto runOnce() -> void;
    auto stop() -> void;
    auto watchPathRecursively(inotifypp::filesystem::path path) -> NotifierBuilder&;
    auto watchFile(inotifypp::filesystem::path file) -> NotifierBuilder&;
    auto unwatchFile(inotifypp::filesystem::path file) -> NotifierBuilder&;
    auto ignoreFileOnce(inotifypp::filesystem::path file) -> NotifierBuilder&;
    auto ignoreFile(inotifypp::filesystem::path file) -> NotifierBuilder&;
    auto onEvent(Event event, EventObserver) -> NotifierBuilder&;
    auto onEvents(std::vector<Event> event, EventObserver) -> NotifierBuilder&;
    auto onUnexpectedEvent(EventObserver) -> NotifierBuilder&;
    auto setEventTimeout(std::chrono::milliseconds timeout, EventObserver eventObserver)
        -> NotifierBuilder&;

  private:
    std::shared_ptr<Inotify> mInotify;
    std::map<Event, EventObserver> mEventObserver;
    EventObserver mUnexpectedEventObserver;
};

NotifierBuilder BuildNotifier();
}
