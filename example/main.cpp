#include <inotify-cpp/NotifierBuilder.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace inotify;

int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::cout << "Usage: ./inotify_example /path/to/dir" << std::endl;
        exit(0);
    }

    // Parse the directory to watch
    boost::filesystem::path path(argv[1]);

    // Set the event handler which will be used to process particular events
    auto handleNotification = [&](Notification notification) {
        std::cout << "Event " << notification.event << " on " << notification.path << " at "
                  << notification.time.time_since_epoch().count() << " was triggered." << std::endl;
    };

    // Set the a separate unexpected event handler for all other events. An exception is thrown by
    // default.
    auto handleUnexpectedNotification = [](Notification notification) {
        std::cout << "Event " << notification.event << " on " << notification.path << " at "
                  << notification.time.time_since_epoch().count()
                  << " was triggered, but was not expected" << std::endl;
    };

    // Set the events to be notified for
    auto events = { Event::open | Event::is_dir, // some events occur in combinations
                    Event::access,
                    Event::create,
                    Event::modify,
                    Event::remove,
                    Event::move };

    // The notifier is configured to watch the parsed path for the defined events. Particular files
    // or paths can be ignored(once).
    auto notifier = BuildNotifier()
                        .watchPathRecursively(path)
                        .ignoreFileOnce("fileIgnoredOnce")
                        .ignoreFile("fileIgnored")
                        .onEvents(events, handleNotification)
                        .onUnexpectedEvent(handleUnexpectedNotification);

    // The event loop is started in a separate thread context.
    std::thread thread([&](){ notifier.run(); });

    // Terminate the event loop after 60 seconds
    std::this_thread::sleep_for(std::chrono::seconds(60));
    notifier.stop();
    thread.join();
    return 0;
}
