#include <inotify-cpp/Event.h>

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace inotify {

bool containsEvent(const Event& allEvents, const Event& event)
{
    if (static_cast<std::uint32_t>(event & allEvents)) {
        return true;
    } else {
        return false;
    }
}

std::ostream& operator<<(std::ostream& stream, const Event& event)
{
    std::string maskString = "";

    if (containsEvent(event, Event::access))
        maskString.append("access ");
    if (containsEvent(event, Event::attrib))
        maskString.append("attrib ");
    if (containsEvent(event, Event::close_write))
        maskString.append("close_write ");
    if (containsEvent(event, Event::close_nowrite))
        maskString.append("close_nowrite ");
    if (containsEvent(event, Event::create))
        maskString.append("create ");
    if (containsEvent(event, Event::remove))
        maskString.append("remove ");
    if (containsEvent(event, Event::remove_self))
        maskString.append("remove_self ");
    if (containsEvent(event, Event::modify))
        maskString.append("modify ");
    if (containsEvent(event, Event::move_self))
        maskString.append("move_self ");
    if (containsEvent(event, Event::moved_from))
        maskString.append("moved_from ");
    if (containsEvent(event, Event::moved_to))
        maskString.append("moved_to ");
    if (containsEvent(event, Event::open))
        maskString.append("open ");
    if (containsEvent(event, Event::is_dir))
        maskString.append("is_dir ");
    if (containsEvent(event, Event::unmount))
        maskString.append("unmount ");
    if (containsEvent(event, Event::q_overflow))
        maskString.append("q_overflow ");
    if (containsEvent(event, Event::close))
        maskString.append("close ");
    if (containsEvent(event, Event::ignored))
        maskString.append("ignored ");
    if (containsEvent(event, Event::oneshot))
        maskString.append("oneshot ");

    stream << maskString;
    return stream;
}
}
