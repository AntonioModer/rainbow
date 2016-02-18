// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/Core.h"

#include "Common/String.h"
#include "FileSystem/Path.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_MACOS
#   define BOOST_FILESYSTEM_NO_DEPRECATED
#   ifdef __GNUC__
#      pragma GCC diagnostic push
#      pragma GCC diagnostic ignored "-Wold-style-cast"
#   endif
#   include <boost/filesystem.hpp>
#   ifdef __GNUC__
#      pragma GCC diagnostic pop
#   endif
namespace filesystem = boost::filesystem;
#else
#   include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif

namespace
{
#if defined(RAINBOW_OS_MACOS)
    constexpr const char kCoreCLRLibrary[] = "libcoreclr.dylib";
#elif defined(RAINBOW_OS_UNIX)
    constexpr const char kCoreCLRLibrary[] = "libcoreclr.so";
#elif defined(RAINBOW_OS_WINDOWS)
    constexpr const char kCoreCLRLibrary[] = "coreclr.dll";
#else
#   error "Unsupported platform"
#endif

    constexpr const char kCoreCLRPath[] = "Runtime";
}

const char* coreclr::core_library_path()
{
    static std::string s_core_library_path =
        (filesystem::path{Path::current()} / kCoreCLRPath / kCoreCLRLibrary)
            .string();
    return s_core_library_path.c_str();
}

const char* coreclr::native_dll_search_directories()
{
    static std::string s_runtime_path =
        (filesystem::path{Path::current()} / kCoreCLRPath).string();
    return s_runtime_path.c_str();
}

const char* coreclr::trusted_platform_assemblies()
{
    static bool s_is_initialized = false;
    static std::string s_assemblies;

    if (!s_is_initialized)
    {
        auto assemblies_path = native_dll_search_directories();
        std::string il_assemblies;
        for (auto&& entry : filesystem::directory_iterator{assemblies_path})
        {
            if (!filesystem::is_regular_file(entry.status()))
                continue;

            const auto& path = entry.path();
            if (path.extension() == ".dll")
            {
                const std::string& stem = path.stem().string();
                if (rainbow::ends_with(rainbow::string_view{stem}, ".ni"))
                    s_assemblies += path.string() + ":";
                else
                    il_assemblies += path.string() + ":";
            }
        }
        s_assemblies += il_assemblies;
        if (!s_assemblies.empty())
        {
            s_assemblies.pop_back();
            s_assemblies.shrink_to_fit();
        }

        s_is_initialized = true;
    }

    return s_assemblies.c_str();
}
