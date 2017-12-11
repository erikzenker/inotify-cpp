#include <inotify/NotifierBuilder.h>

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <future>
#include <fstream>

using namespace inofity;

void openFile(boost::filesystem::path file){
    std::ifstream stream;
    stream.open (file.string(), std::ifstream::in);
    BOOST_CHECK(stream.is_open());
    stream.close();
}

struct NotifierBuilderTests {
  NotifierBuilderTests() : testFile_("test.txt"), timeout_(1) {
    boost::filesystem::ofstream ofstream{testFile_};

  }
  ~NotifierBuilderTests() = default;

  boost::filesystem::path testFile_;
  std::chrono::seconds timeout_;


  // Events
  std::promise<Notification> promisedOpen_;
  std::promise<Notification> promisedCloseNoWrite_;
};

BOOST_FIXTURE_TEST_CASE(shouldNotAcceptNotExistingPaths, NotifierBuilderTests)
{
    BOOST_CHECK_THROW(BuildNotifier().watchPathRecursively("/not/existing/path/"), std::invalid_argument);
    BOOST_CHECK_THROW(BuildNotifier().watchFile("/not/existing/file"), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnOpenEvent, NotifierBuilderTests)
{
    auto notifier = BuildNotifier()
            .watchFile(testFile_)
            .onEvent(Event::open, [&](Notification notification){
                promisedOpen_.set_value(notification);
            });

    std::thread thread([&notifier](){notifier.run_once();});

    openFile(testFile_);

    auto futureOpenEvent = promisedOpen_.get_future();
    BOOST_CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureOpenEvent.get().event == Event::open);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnMultipleEvents, NotifierBuilderTests)
{
    auto notifier = BuildNotifier()
        .watchFile(testFile_)
        .onEvents({Event::open, Event::close_nowrite}, [&](Notification notification){
            switch(notification.event){
                case Event::open:
                    promisedOpen_.set_value(notification);
                    break;
                case Event::close_nowrite:
                    promisedCloseNoWrite_.set_value(notification);
                    break;
                default:
                  BOOST_FAIL("Unexpected event");
            }
        });

    std::thread thread([&notifier]() {
      notifier.run_once();
      notifier.run_once();
    });

    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    auto futureCloseNoWrite = promisedCloseNoWrite_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureOpen.get().event == Event::open);
    BOOST_CHECK(futureCloseNoWrite.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureCloseNoWrite.get().event == Event::close_nowrite);
    thread.join();
}
