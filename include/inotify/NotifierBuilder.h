/**
 * @file      Inotify.h
 * @author    Erik Zenker
 * @date      20.11.2017
 * @copyright MIT
 **/
#pragma once

#include <inotify/Inotify.h>

#include <boost/filesystem.hpp>

#include <string>
#include <memory>


namespace inofity {



    enum class Event {
        access = IN_ACCESS,
        attrib = IN_ATTRIB,
        close_write = IN_CLOSE_WRITE,
        close_nowrite = IN_CLOSE_NOWRITE,
        create = IN_CREATE,
        remove = IN_DELETE,
        remove_self = IN_DELETE_SELF,
        modify = IN_MODIFY,
        move_self = IN_MOVE_SELF,
        moved_from = IN_MOVED_FROM,
        moved_to = IN_MOVED_TO,
        move = IN_MOVE,
        open = IN_OPEN,
        all = IN_ALL_EVENTS,
        close = IN_CLOSE
    };

    using EventObserver = std::function<void(Event)>;

    class NotifierBuilder {
    public:
        NotifierBuilder() : mInotify(std::make_shared<Inotify>())
        {
        }

        auto run() -> void;
        auto run_once() -> void;
        auto notifyOn(std::vector<Event> events) -> NotifierBuilder&;
        auto watchPathRecursively(boost::filesystem::path path) -> NotifierBuilder&;
        auto watchFile(boost::filesystem::path file) -> NotifierBuilder&;
        auto ignoreFileOnce(std::string fileName) -> NotifierBuilder&;
        auto onEvent(Event event, std::function<void(Event)>) -> NotifierBuilder&;

    private:
        std::shared_ptr<Inotify> mInotify;
        std::map<Event, EventObserver> mEventObserver;
    };

    NotifierBuilder BuildNotifier() { return {};};

    auto NotifierBuilder::notifyOn(std::vector<Event> events) -> NotifierBuilder&
    {
        std::uint32_t eventMask(0);
        for(auto event : events){
            eventMask = eventMask | static_cast<std::uint32_t>(event);
        }

        mInotify->setEventMask(eventMask);
        return *this;
    }

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

    auto NotifierBuilder::ignoreFileOnce(std::string fileName) -> NotifierBuilder&
    {
        mInotify->ignoreFileOnce(fileName);
        return *this;
    }

    auto NotifierBuilder::onEvent(Event event, EventObserver eventObserver) -> NotifierBuilder&
    {
        mEventObserver[event] = eventObserver;
        return *this;
    }

    auto NotifierBuilder::run_once() -> void
    {
        auto mask = mInotify->getNextEvent().mask;
        Event event = static_cast<Event>(mask);

        auto eventAndEventObserver = mEventObserver.find(event);
        if (eventAndEventObserver == mEventObserver.end()) {
            return;
        }

        auto eventObserver = eventAndEventObserver->second;
        eventObserver(event);
    }


    auto NotifierBuilder::run() -> void
    {
        while(true) {
            run_once();
        }
    }
}