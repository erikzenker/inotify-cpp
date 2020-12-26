#pragma once

#include <inotify-cpp/Inotify.h>
#include <inotify-cpp/Notification.h>

#include <filesystem>

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
    auto watchPathRecursively(std::filesystem::path path) -> NotifierBuilder&;
    auto watchFile(std::filesystem::path file) -> NotifierBuilder&;
    auto unwatchFile(std::filesystem::path file) -> NotifierBuilder&;
    auto ignoreFileOnce(std::filesystem::path file) -> NotifierBuilder&;
    auto ignoreFile(std::filesystem::path file) -> NotifierBuilder&;
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
