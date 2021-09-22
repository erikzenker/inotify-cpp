#pragma once

#if __cplusplus >= 201703L && !defined(USE_BOOST_FILESYSTEM)

#include <filesystem>
#include <optional>

namespace inotifypp
{
    namespace filesystem = std::filesystem;

    using error_code = std::error_code;

    template<typename T>
    using optional = std::optional<T>;

    inline constexpr auto nullopt() { return std::nullopt; }

}

#else

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

namespace inotifypp
{
    namespace filesystem = boost::filesystem;

    using error_code = boost::system::error_code;

    template<typename T>
    using optional = boost::optional<T>;

    inline constexpr boost::none_t nullopt() { return boost::none; };
}

#endif
