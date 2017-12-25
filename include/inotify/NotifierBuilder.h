/**
 * @file      Inotify.h
 * @author    Erik Zenker
 * @date      20.11.2017
 * @copyright MIT
 **/
#pragma once

#include <inotify/Inotify.h>

#include <boost/filesystem.hpp>

#include <memory>
#include <string>

namespace inofity {

enum class Event {
    access = IN_ACCESS,
    attrib = IN_ATTRIB,
    close_write = IN_CLOSE_WRITE,
    close_nowrite = IN_CLOSE_NOWRITE,
    close = IN_CLOSE,
    create = IN_CREATE,
    remove = IN_DELETE,
    remove_self = IN_DELETE_SELF,
    modify = IN_MODIFY,
    move_self = IN_MOVE_SELF,
    moved_from = IN_MOVED_FROM,
    moved_to = IN_MOVED_TO,
    move = IN_MOVE,
    open = IN_OPEN,
    all = IN_ALL_EVENTS
};

struct Notification {
    Event event;
    boost::filesystem::path path;
};

std::ostream& operator<<(std::ostream& stream, const Event& event)
{
    switch (event) {
    case Event::access:
        stream << "access(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::attrib:
        stream << "attrib(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::close_write:
        stream << "close_write(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::close_nowrite:
        stream << "close_nowrite(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::create:
        stream << "close_nowrite(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::remove:
        stream << "remove(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::remove_self:
        stream << "remove_self(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::close:
        stream << "close(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::modify:
        stream << "modify(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::move_self:
        stream << "move_self(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::moved_from:
        stream << "moved_from(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::moved_to:
        stream << "moved_to(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::move:
        stream << "move(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::open:
        stream << "open(" << static_cast<uint32_t>(event) << ")";
        break;
    case Event::all:
        stream << "all(" << static_cast<uint32_t>(event) << ")";
        break;
    default:
        throw std::runtime_error("Unknown inotify event");
    }
    return stream;
}

using EventObserver = std::function<void(Notification)>;

class NotifierBuilder {
  public:
    NotifierBuilder();

    auto run() -> void;
    auto run_once() -> bool;
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

NotifierBuilder::NotifierBuilder()
    : mInotify(std::make_shared<Inotify>())
    , mUnexpectedEventObserver([](Notification notification) {
        std::stringstream ss;
        ss << "Unexpected event " << notification.event << " on " << notification.path;
        throw std::runtime_error(ss.str());
    })

{
}

NotifierBuilder BuildNotifier()
{
    return {};
};

auto NotifierBuilder::watchPathRecursively(boost::filesystem::path path) -> NotifierBuilder&
{
    mInotify->watchDirectoryRecursively(path);
    return *this;
}

auto NotifierBuilder::watchFile(boost::filesystem::path file) -> NotifierBuilder&
{
    mInotify->watchFile(file);
    return *this;
}

auto NotifierBuilder::ignoreFileOnce(boost::filesystem::path file) -> NotifierBuilder&
{
    mInotify->ignoreFileOnce(file.string());
    return *this;
}

auto NotifierBuilder::onEvent(Event event, EventObserver eventObserver) -> NotifierBuilder&
{
    mInotify->setEventMask(mInotify->getEventMask() | static_cast<std::uint32_t>(event));
    mEventObserver[event] = eventObserver;
    return *this;
}

auto NotifierBuilder::onEvents(std::vector<Event> events, EventObserver eventObserver)
    -> NotifierBuilder&
{
    for (auto event : events) {
        mInotify->setEventMask(mInotify->getEventMask() | static_cast<std::uint32_t>(event));
        mEventObserver[event] = eventObserver;
    }

    return *this;
}

auto NotifierBuilder::onUnexpectedEvent(EventObserver eventObserver) -> NotifierBuilder&
{
    mUnexpectedEventObserver = eventObserver;
    return *this;
}

auto NotifierBuilder::run_once() -> bool
{
    auto fileSystemEvent = mInotify->getNextEvent();
    if (!fileSystemEvent) {
        return false;
    }

    Event event = static_cast<Event>(fileSystemEvent->mask);

    Notification notification;
    notification.event = event;
    notification.path = fileSystemEvent->path;

    auto eventAndEventObserver = mEventObserver.find(event);
    if (eventAndEventObserver == mEventObserver.end()) {
        mUnexpectedEventObserver(notification);
        return true;
    }


    auto eventObserver = eventAndEventObserver->second;
    eventObserver(notification);
}

auto NotifierBuilder::run() -> void
{
    while (true) {
        if (!run_once()) {
            return;
        }
    }
}

auto NotifierBuilder::stop() -> void
{
    mInotify->stop();
}
}