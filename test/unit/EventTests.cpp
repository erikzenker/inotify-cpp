#include <boost/test/unit_test.hpp>

#include <inotify-cpp/Event.h>

#include <sstream>

using namespace inotify;

BOOST_AUTO_TEST_CASE(shouldCalculateEventsCorrectly)
{
    BOOST_CHECK_EQUAL(8, static_cast<std::uint32_t>(Event::close_write));
    BOOST_CHECK_EQUAL(16, static_cast<std::uint32_t>(Event::close_nowrite));
    BOOST_CHECK_EQUAL(24, static_cast<std::uint32_t>(Event::close));
    BOOST_CHECK_EQUAL(Event::close, Event::close_nowrite | Event::close_write);
}

BOOST_AUTO_TEST_CASE(shouldOutputEventsCorrectly)
{
    std::stringstream eventSStream;
    eventSStream << Event::close;
    BOOST_CHECK_EQUAL("close_write close_nowrite close ", eventSStream.str());
}
