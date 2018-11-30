#pragma once
#include <cstdint>
#include <iostream>

#include <sys/inotify.h>

namespace inotify {

enum class Event : std::uint32_t {
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
    is_dir = IN_ISDIR,
    unmount = IN_UNMOUNT,
    q_overflow = IN_Q_OVERFLOW,
    ignored = IN_IGNORED,
    oneshot = IN_ONESHOT,
    all = IN_ALL_EVENTS
};
constexpr Event operator&(Event lhs, Event rhs)
{
    return static_cast<Event>(
        static_cast<std::underlying_type<Event>::type>(lhs)
        & static_cast<std::underlying_type<Event>::type>(rhs));
}

constexpr Event operator|(Event lhs, Event rhs)
{
    return static_cast<Event>(
        static_cast<std::underlying_type<Event>::type>(lhs)
        | static_cast<std::underlying_type<Event>::type>(rhs));
}

std::ostream& operator<<(std::ostream& stream, const Event& event);
bool containsEvent(const Event& allEvents, const Event& event);
}