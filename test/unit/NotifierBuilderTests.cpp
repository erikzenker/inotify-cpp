#include <inotify-cpp/NotifierBuilder.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>

using namespace inotify;

void openFile(const boost::filesystem::path& file)
{
    std::ifstream stream;
    stream.open(file.string(), std::ifstream::in);
    BOOST_CHECK(stream.is_open());
    stream.close();
}

void createFile(const boost::filesystem::path& file)
{
    boost::filesystem::ofstream stream(file);
}

void removeFile(const boost::filesystem::path& file)
{
    boost::filesystem::remove(file);
}

struct NotifierBuilderTests {
    NotifierBuilderTests()
        : testDirectory_("testDirectory")
        , recursiveTestDirectory_(testDirectory_ / "recursiveTestDirectory")
        , testFile_(testDirectory_ / "test.txt")
        , recursiveTestFile_(recursiveTestDirectory_ / "recursiveTest.txt")
        , createdFile_(testDirectory_ / "created.txt")
        , timeout_(1)
    {
        boost::filesystem::create_directories(testDirectory_);
        boost::filesystem::create_directories(recursiveTestDirectory_);

        removeFile(testFile_);
        removeFile(recursiveTestFile_);
        removeFile(createdFile_);
        createFile(testFile_);
        createFile(recursiveTestFile_);
    }
    ~NotifierBuilderTests() = default;

    boost::filesystem::path testDirectory_;
    boost::filesystem::path recursiveTestDirectory_;
    boost::filesystem::path testFile_;
    boost::filesystem::path recursiveTestFile_;
    boost::filesystem::path createdFile_;

    std::chrono::seconds timeout_;

    // Events
    std::promise<Notification> promisedOpen_;
    std::promise<Notification> promisedCreate_;
    std::promise<Notification> promisedCloseNoWrite_;
    std::promise<Notification> promisedCombinedEvent_;
};

BOOST_FIXTURE_TEST_CASE(shouldNotAcceptNotExistingPaths, NotifierBuilderTests)
{
    BOOST_CHECK_THROW(
        BuildNotifier().watchPathRecursively("/not/existing/path/"), std::invalid_argument);
    BOOST_CHECK_THROW(BuildNotifier().watchFile("/not/existing/file"), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(shouldStopNotifierLoop, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_);

    std::thread thread([&notifier]() { notifier.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds{100});

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnOpenEvent, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_).onEvent(
        Event::open, [&](Notification notification) { promisedOpen_.set_value(notification); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    auto futureOpenEvent = promisedOpen_.get_future();
    BOOST_CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureOpenEvent.get().event == Event::open);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnAllEvents, NotifierBuilderTests)
{
    auto notifier
        = BuildNotifier()
              .watchFile(testFile_)
              .onEvent(
                  Event::all,
                  [&](Notification notification) {
                      switch (notification.event) {
                      default:
                          BOOST_ASSERT_MSG(false, "All events should be handled");
                      case Event::open:
                          promisedOpen_.set_value(notification);
                          break;
                      case Event::close_nowrite:
                          promisedCombinedEvent_.set_value(notification);
                          break;
                      };
                  })
              .onUnexpectedEvent([](Notification) {
                  BOOST_ASSERT_MSG(false, "All events should be catched by event observer");
              });

    std::thread thread([&notifier]() { notifier.run(); });

    openFile(testFile_);

    auto futureOpenEvent = promisedOpen_.get_future();
    auto futureCombinedEvent = promisedCombinedEvent_.get_future();
    BOOST_CHECK(futureOpenEvent.wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(futureCombinedEvent.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnCombinedEvent, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_).onEvent(
        Event::close_nowrite | Event::close,
        [&](Notification notification) { promisedCombinedEvent_.set_value(notification); });

    std::thread thread([&notifier]() { notifier.run(); });

    openFile(testFile_);

    auto futureCombinedEvent = promisedCombinedEvent_.get_future();
    BOOST_CHECK(futureCombinedEvent.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotifyOnMultipleEvents, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_).onEvents(
        { Event::open, Event::close_nowrite }, [&](Notification notification) {
            switch (notification.event) {
            default:
                break;
            case Event::open:
                promisedOpen_.set_value(notification);
                break;
            case Event::close_nowrite:
                promisedCloseNoWrite_.set_value(notification);
                break;
            }
        });

    std::thread thread([&notifier]() {
        notifier.runOnce();
        notifier.runOnce();
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

BOOST_FIXTURE_TEST_CASE(shouldStopRunOnce, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldStopRun, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_);

    std::thread thread([&notifier]() { notifier.run(); });

    notifier.stop();

    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFileOnce, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_).ignoreFileOnce(testFile_).onEvent(
        Event::open, [&](Notification notification) { promisedOpen_.set_value(notification); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) != std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldIgnoreFile, NotifierBuilderTests)
{
    auto notifier = BuildNotifier().watchFile(testFile_).ignoreFile(testFile_).onEvent(
        Event::open, [&](Notification notification) { promisedOpen_.set_value(notification); });

    std::thread thread([&notifier]() { notifier.run(); });

    openFile(testFile_);
    openFile(testFile_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) != std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldWatchPathRecursively, NotifierBuilderTests)
{

    auto notifier = BuildNotifier()
                        .watchPathRecursively(testDirectory_)
                        .onEvent(Event::open, [&](Notification notification) {
                            switch (notification.event) {
                            default:
                                break;
                            case Event::open:
                                if (notification.path == recursiveTestFile_) {
                                    promisedOpen_.set_value(notification);
                                }
                                break;
                            }
                        });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(recursiveTestFile_);

    auto futureOpen = promisedOpen_.get_future();
    BOOST_CHECK(futureOpen.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldNotTriggerEventsOnWatchRecursively, NotifierBuilderTests)
{
    auto notifier = BuildNotifier()
            .watchPathRecursively(testDirectory_)
            .onEvent(Event::all, [&](Notification) {
                BOOST_ASSERT_MSG(false, "Events should not be triggered when watching a directory recursively.");
            });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    std::this_thread::sleep_for(std::chrono::milliseconds{1000});

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldWatchCreatedFile, NotifierBuilderTests)
{

    auto notifier = BuildNotifier().watchPathRecursively(testDirectory_);

    notifier
        .onEvent(
            Event::create,
            [&](Notification notification) {
                notifier.watchFile(notification.path);
                promisedCreate_.set_value(notification);
            })
        .onEvent(Event::close_nowrite, [&](Notification notification) {
            promisedCloseNoWrite_.set_value(notification);
        });

    std::thread thread([&notifier]() { notifier.run(); });

    createFile(createdFile_);
    openFile(createdFile_);

    auto futureCreate = promisedCreate_.get_future();
    BOOST_CHECK(futureCreate.wait_for(timeout_) == std::future_status::ready);
    auto futureCloseNoWrite = promisedCloseNoWrite_.get_future();
    BOOST_CHECK(futureCloseNoWrite.wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldUnwatchPath, NotifierBuilderTests)
{
    std::promise<Notification> timeoutObserved;

    auto notifier = BuildNotifier().watchFile(testFile_).unwatchFile(testFile_);

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);
    BOOST_CHECK(promisedOpen_.get_future().wait_for(timeout_) != std::future_status::ready);
    notifier.stop();
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldCallUserDefinedUnexpectedExceptionObserver, NotifierBuilderTests)
{
    std::promise<void> observerCalled;

    auto notifier = BuildNotifier().watchFile(testFile_).onUnexpectedEvent(
        [&](Notification) { observerCalled.set_value(); });

    std::thread thread([&notifier]() { notifier.runOnce(); });

    openFile(testFile_);

    BOOST_CHECK(observerCalled.get_future().wait_for(timeout_) == std::future_status::ready);
    thread.join();
}

BOOST_FIXTURE_TEST_CASE(shouldSetEventTimeout, NotifierBuilderTests)
{
    std::promise<Notification> timeoutObserved;
    std::chrono::milliseconds timeout(100);

    auto notifier
        = BuildNotifier()
              .watchFile(testFile_)
              .onEvent(
                  Event::open,
                  [&](Notification notification) {
                      promisedOpen_.set_value(notification); })
              .setEventTimeout(timeout, [&](Notification notification) {
                  timeoutObserved.set_value(notification);
              });

    std::thread thread([&notifier]() {
        notifier.run(); // open
    });

    openFile(testFile_);

    BOOST_CHECK(promisedOpen_.get_future().wait_for(timeout_) == std::future_status::ready);
    BOOST_CHECK(timeoutObserved.get_future().wait_for(timeout_) == std::future_status::ready);

    notifier.stop();
    thread.join();
}
