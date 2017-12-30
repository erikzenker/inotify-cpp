#include <inotify-cpp/NotifierBuilder.h>

namespace inotify {

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

auto NotifierBuilder::runOnce() -> bool
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
        if (!runOnce()) {
            return;
        }
    }
}

auto NotifierBuilder::stop() -> void
{
    mInotify->stop();
}
}
