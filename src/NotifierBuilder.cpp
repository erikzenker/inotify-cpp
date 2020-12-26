
#include <inotify-cpp/NotifierBuilder.h>

namespace inotify {

NotifierBuilder::NotifierBuilder()
    : mInotify(std::make_shared<Inotify>())
{
}

NotifierBuilder BuildNotifier()
{
    return {};
}

auto NotifierBuilder::watchPathRecursively(std::filesystem::path path) -> NotifierBuilder&
{
    mInotify->watchDirectoryRecursively(path);
    return *this;
}

auto NotifierBuilder::watchFile(std::filesystem::path file) -> NotifierBuilder&
{
    mInotify->watchFile(file);
    return *this;
}

auto NotifierBuilder::unwatchFile(std::filesystem::path file) -> NotifierBuilder&
{
    mInotify->unwatchFile(file);
    return *this;
}

auto NotifierBuilder::ignoreFileOnce(std::filesystem::path file) -> NotifierBuilder&
{
    mInotify->ignoreFileOnce(file.string());
    return *this;
}

auto NotifierBuilder::ignoreFile(std::filesystem::path file) -> NotifierBuilder&
{
    mInotify->ignoreFile(file.string());
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
/**
 * Sets the time between two successive events. Events occurring in between
 * will be ignored and the event observer will be called.
 *
 * @param timeout
 * @param eventObserver
 * @return
 */
auto NotifierBuilder::setEventTimeout(
    std::chrono::milliseconds timeout, EventObserver eventObserver) -> NotifierBuilder&
{
    auto onEventTimeout = [eventObserver](FileSystemEvent fileSystemEvent) {
        Notification notification { static_cast<Event>(fileSystemEvent.mask),
                                    fileSystemEvent.path,
                                    fileSystemEvent.eventTime };
        eventObserver(notification);
    };

    mInotify->setEventTimeout(timeout, onEventTimeout);
    return *this;
}

auto NotifierBuilder::runOnce() -> void
{
    auto fileSystemEvent = mInotify->getNextEvent();
    if (!fileSystemEvent) {
        return;
    }

    Event currentEvent = static_cast<Event>(fileSystemEvent->mask);

    Notification notification { currentEvent, fileSystemEvent->path, fileSystemEvent->eventTime };

    for (auto& eventAndEventObserver : mEventObserver) {
        auto& event = eventAndEventObserver.first;
        auto& eventObserver = eventAndEventObserver.second;

        if (event == Event::all) {
            eventObserver(notification);
            return;
        }

        if (event == currentEvent) {
            eventObserver(notification);
            return;
        }
    }

    if (mUnexpectedEventObserver) {
        mUnexpectedEventObserver(notification);
    }
}

auto NotifierBuilder::run() -> void
{
    while (true) {
        if (mInotify->hasStopped()) {
          break;
        }

        runOnce();
    }
}

auto NotifierBuilder::stop() -> void
{
    mInotify->stop();
}
}
