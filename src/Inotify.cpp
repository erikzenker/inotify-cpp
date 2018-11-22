#include <inotify-cpp/Inotify.h>

#include <string>
#include <vector>

namespace inotify {

Inotify::Inotify()
    : mError(0)
    , mEventTimeout(0)
    , mLastEventTime(std::chrono::steady_clock::now())
    , mEventMask(IN_ALL_EVENTS)
    , mThreadSleep(250)
    , mIgnoredDirectories(std::vector<std::string>())
    , mInotifyFd(0)
    , mOnEventTimeout([](FileSystemEvent) {})
    , mEventBuffer(MAX_EVENTS * (EVENT_SIZE + 16), 0)
{
    // Initialize inotify
    init();
}

Inotify::~Inotify()
{
    if (!close(mInotifyFd)) {
        mError = errno;
    }
}

void Inotify::init()
{
    mStopped = false;
    mInotifyFd = inotify_init1(IN_NONBLOCK);
    if (mInotifyFd == -1) {
        mError = errno;
        std::stringstream errorStream;
        errorStream << "Can't initialize inotify ! " << strerror(mError) << ".";
        throw std::runtime_error(errorStream.str());
    }
}

/**
 * @brief Adds the given path and all files and subdirectories
 *        to the set of watched files/directories.
 *        Symlinks will be followed!
 *
 * @param path that will be watched recursively
 *
 */
void Inotify::watchDirectoryRecursively(fs::path path)
{
    if (fs::exists(path)) {
        if (fs::is_directory(path)) {
            fs::recursive_directory_iterator it(path, fs::symlink_option::recurse);
            fs::recursive_directory_iterator end;

            while (it != end) {
                fs::path currentPath = *it;

                if (fs::is_directory(currentPath)) {
                    watchFile(currentPath);
                }
                if (fs::is_symlink(currentPath)) {
                    watchFile(currentPath);
                }
                ++it;
            }
        }
        watchFile(path);
    } else {
        throw std::invalid_argument(
            "Can´t watch Path! Path does not exist. Path: " + path.string());
    }
}

/**
 * @brief Adds a single file/directorie to the list of
 *        watches. Path and corresponding watchdescriptor
 *        will be stored in the directorieMap. This is done
 *        because events on watches just return this
 *        watchdescriptor.
 *
 * @param path that will be watched
 *
 */
void Inotify::watchFile(fs::path filePath)
{
    if (fs::exists(filePath)) {
        mError = 0;
        int wd = 0;
        if (!isIgnored(filePath.string())) {
            wd = inotify_add_watch(mInotifyFd, filePath.string().c_str(), mEventMask);
        }

        if (wd == -1) {
            mError = errno;
            std::stringstream errorStream;
            if (mError == 28) {
                errorStream << "Failed to watch! " << strerror(mError)
                            << ". Please increase number of watches in "
                               "\"/proc/sys/fs/inotify/max_user_watches\".";
                throw std::runtime_error(errorStream.str());
            }

            errorStream << "Failed to watch! " << strerror(mError)
                        << ". Path: " << filePath.string();
            throw std::runtime_error(errorStream.str());
        }
        mDirectorieMap.left.insert({wd, filePath});
    } else {
        throw std::invalid_argument(
            "Can´t watch Path! Path does not exist. Path: " + filePath.string());
    }
}

void Inotify::ignoreFileOnce(fs::path file)
{
    mOnceIgnoredDirectories.push_back(file.string());
}

void Inotify::ignoreFile(fs::path file)
{
    mIgnoredDirectories.push_back(file.string());
}


void Inotify::unwatchFile(fs::path file)
{
    removeWatch(mDirectorieMap.right.at(file));
}

/**
 * @brief Removes watch from set of watches. This
 *        is not done recursively!
 *
 * @param wd watchdescriptor
 *
 */
void Inotify::removeWatch(int wd)
{
    int result = inotify_rm_watch(mInotifyFd, wd);
    if (result == -1) {
        mError = errno;
        std::stringstream errorStream;
        errorStream << "Failed to remove watch! " << strerror(mError) << ".";
        throw std::runtime_error(errorStream.str());
    }
}

fs::path Inotify::wdToPath(int wd)
{
    return mDirectorieMap.left.at(wd);
}

void Inotify::setEventMask(uint32_t eventMask)
{
    mEventMask = eventMask;
}

uint32_t Inotify::getEventMask()
{
    return mEventMask;
}

void Inotify::setEventTimeout(
    std::chrono::milliseconds eventTimeout, std::function<void(FileSystemEvent)> onEventTimeout)
{
    mEventTimeout = eventTimeout;
    mOnEventTimeout = onEventTimeout;
}

/**
 * @brief Blocking wait on new events of watched files/directories
 *        specified on the eventmask. FileSystemEvents
 *        will be returned one by one. Thus this
 *        function can be called in some while(true)
 *        loop.
 *
 * @return A new FileSystemEvent
 *
 */
boost::optional<FileSystemEvent> Inotify::getNextEvent()
{
    std::vector<FileSystemEvent> newEvents;

    while (mEventQueue.empty() && !mStopped) {
        auto length = readEventsIntoBuffer(mEventBuffer);
        readEventsFromBuffer(mEventBuffer.data(), length, newEvents);
        filterEvents(newEvents, mEventQueue);
    }

    if (mStopped) {
        return boost::none;
    }

    auto event = mEventQueue.front();
    mEventQueue.pop();
    return event;
}

void Inotify::stop()
{
    mStopped = true;
}

bool Inotify::hasStopped()
{
    return mStopped;
}

bool Inotify::isIgnored(std::string file)
{
    for (unsigned i = 0; i < mOnceIgnoredDirectories.size(); ++i) {
        size_t pos = file.find(mOnceIgnoredDirectories[i]);
        if (pos != std::string::npos) {
            mOnceIgnoredDirectories.erase(mOnceIgnoredDirectories.begin() + i);
            return true;
        }
    }

    for (unsigned i = 0; i < mIgnoredDirectories.size(); ++i) {
        size_t pos = file.find(mIgnoredDirectories[i]);
        if (pos != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool Inotify::isOnTimeout(const std::chrono::steady_clock::time_point &eventTime)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(eventTime - mLastEventTime) < mEventTimeout;
}

ssize_t Inotify::readEventsIntoBuffer(std::vector<uint8_t>& eventBuffer)
{
    ssize_t length = 0;
    length = 0;
    while (length <= 0 && !mStopped) {
        std::this_thread::sleep_for(std::chrono::milliseconds(mThreadSleep));

        length = read(mInotifyFd, eventBuffer.data(), eventBuffer.size());
        if (length == -1) {
            mError = errno;
            if (mError != EINTR) {
                continue;
            }
        }
    }

    return length;
}

void Inotify::readEventsFromBuffer(
    uint8_t* buffer, int length, std::vector<inotify::FileSystemEvent>& events)
{
    int i = 0;
    while (i < length) {
        inotify_event* event = ((struct inotify_event*)&buffer[i]);

        if(event->mask & IN_IGNORED){
            i += EVENT_SIZE + event->len;
            mDirectorieMap.left.erase(event->wd);
            continue;
        }

        auto path = wdToPath(event->wd) / std::string(event->name);

        if (fs::is_directory(path)) {
            event->mask |= IN_ISDIR;
        }
        FileSystemEvent fsEvent(event->wd, event->mask, path, std::chrono::steady_clock::now());

        if (!fsEvent.path.empty()) {
            events.push_back(fsEvent);

        } else {
            // Event is not complete --> ignore
        }

        i += EVENT_SIZE + event->len;
    }
}

void Inotify::filterEvents(
    std::vector<inotify::FileSystemEvent>& events, std::queue<FileSystemEvent>& eventQueue)
{
    for (auto eventIt = events.begin(); eventIt < events.end();) {
        FileSystemEvent currentEvent = *eventIt;
        if (isOnTimeout(currentEvent.eventTime)) {
            eventIt = events.erase(eventIt);
            mOnEventTimeout(currentEvent);

        } else if (isIgnored(currentEvent.path.string())) {
            eventIt = events.erase(eventIt);
        } else {
            mLastEventTime = currentEvent.eventTime;
            eventQueue.push(currentEvent);
            eventIt++;
        }
    }
}
}
