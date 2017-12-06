#include <inotify/NotifierBuilder.h>

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <future>
#include <fstream>

using namespace inofity;

void openFile(boost::filesystem::path file){
    std::ifstream stream;
    std::cout << file.string() << std::endl;
    stream.open (file.string(), std::ifstream::in);
    BOOST_CHECK(stream.is_open());
    stream.close();
}

BOOST_AUTO_TEST_CASE(shouldNotAcceptNotExistingPaths)
{
    BOOST_CHECK_THROW(BuildNotifier().watchPathRecursively("/not/existing/path/"), std::invalid_argument);
    BOOST_CHECK_THROW(BuildNotifier().watchFile("/not/existing/file"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(shouldNotifyOnOpenEvent)
{
    auto timeout = std::chrono::seconds(100);
    auto file = boost::filesystem::path("/home/erik/projects/Inotify/test/assets/test.txt");
    std::promise<Event> promisedOpenEvent;
    auto futureOpenEvent = promisedOpenEvent.get_future();

    auto notifier = BuildNotifier()
            .watchFile(file)
            .notifyOn({Event::open})
            .onEvent(Event::open, [&promisedOpenEvent](Event e){
                promisedOpenEvent.set_value(e);
            });

    std::thread thread([&notifier](){notifier.run_once();});

    std::this_thread::sleep_for(std::chrono::seconds(1));
    openFile(file);

    BOOST_CHECK(futureOpenEvent.wait_for(timeout) == std::future_status::ready);
    BOOST_CHECK(futureOpenEvent.get() == Event::open);
    thread.join();
}
