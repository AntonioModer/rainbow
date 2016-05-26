// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Path.h"

#include <cerrno>
#include <cstring>
#include <sys/stat.h>

#ifdef RAINBOW_OS_WINDOWS
#   include <Windows.h>
#   include <direct.h>
#   define S_IRWXU 0000700
#   define S_IRWXG 0000070
#   define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)  // directory
#   define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)  // regular file
#   define getcwd(buf, size) _getcwd(buf, size)
#   define kPathSeparator '\\'
#   define kPathSeparatorLiteral "\\"
#else
#   ifdef RAINBOW_OS_ANDROID
#       include <android/native_activity.h>
#   endif
#   include <unistd.h>
#   define kPathSeparator '/'
#   define kPathSeparatorLiteral "/"
#endif

#include "Common/Logging.h"

#define kErrorAccessDirectory   "Error accessing '%s' (%x)"
#define kErrorCreateDirectory   "Failed to create directory '%s' (%x)"
#define kErrorInvalidDirectory  "'%s' is not a valid directory"
#define kUserDataPath           "user"

#ifdef RAINBOW_OS_ANDROID
extern ANativeActivity* g_native_activity;
#endif

namespace
{
    Path g_current_path;
    Path g_executable_path;
    Path g_user_data_path;

#ifdef RAINBOW_OS_WINDOWS
    using mode_t = int;

    int mkdir(const char* dirname, mode_t)
    {
        return _mkdir(dirname);
    }
#endif

    /// Creates a directory, including any intermediate directories.
    /// \see mkdir()
    /// \see stat()
    /// \param path  Directory path to create.
    /// \param mode  Permission bits to set on created directories.
    /// \return 0 upon success. Otherwise, -1 is returned and errno is set to
    ///         indicate the error.
#ifdef RAINBOW_OS_IOS
    int create_directories(const char* path, mode_t mode)
        __attribute__((unused));
#endif
    int create_directories(const char* path, mode_t mode)
    {
        bool end = false;
        struct stat sb;
        char dir[256];
        strcpy(dir, path);
        for (int i = 1; !end; ++i)
        {
            end = dir[i] == '\0';
            if (dir[i] == kPathSeparator || end)
            {
#ifdef RAINBOW_OS_WINDOWS
                // Windows' stat() doesn't handle trailing backslash.
                if (dir[i - 1] == ':' || dir[i - 1] == kPathSeparator)
                    continue;
#endif
                dir[i] = '\0';
                if (stat(dir, &sb) != 0)
                {
                    if (errno != ENOENT)
                    {
                        LOGE(kErrorAccessDirectory, dir, errno);
                        return -1;
                    }
                    if (mkdir(dir, mode) != 0 || stat(dir, &sb) != 0)
                    {
                        LOGE(kErrorCreateDirectory, dir, errno);
                        return -1;
                    }
                }
                if (!S_ISDIR(sb.st_mode))
                {
                    LOGE(kErrorInvalidDirectory, dir);
                    return -1;
                }
                dir[i] = kPathSeparator;
            }
        }
        return 0;
    }
}

const char* Path::basename(const char* path)
{
    auto basename = path;
    for (auto c = path; *c; ++c)
    {
        switch (*c)
        {
            case '/':
            case ':':
            case '\\':
                basename = ++c;
                break;
            default:
                break;
        }
    }
    return basename;
}

const char* Path::current()
{
    return g_current_path;
}

const char* Path::executable_path()
{
    return g_executable_path;
}

void Path::set_current()
{
#ifdef RAINBOW_OS_ANDROID
    const char* data_path = g_native_activity->externalDataPath;
    if (!data_path)
    {
        data_path = g_native_activity->internalDataPath;
        if (!data_path)
            return;
    }
    g_user_data_path = data_path;
#else
    g_user_data_path = g_current_path;
    g_user_data_path += kUserDataPath;
#endif
}

void Path::set_current(const char* path)
{
    g_current_path = path;
    set_current();
}

void Path::set_executable_path(const char* path)
{
#ifdef RAINBOW_OS_WINDOWS
    char absolute_path[MAX_PATH];
    GetFullPathName(path, MAX_PATH, absolute_path, nullptr);
#else
    char absolute_path[PATH_MAX];
    realpath(path, absolute_path);
#endif
    g_executable_path = absolute_path;
}

Path::Path()
{
    if (getcwd(path_, sizeof(path_)) != path_)
        path_[0] = '\0';
}

Path::Path(const char* file, RelativeTo rel)
{
    path_[0] = '\0';
    switch (rel)
    {
        case RelativeTo::CurrentPath: {
#if defined(RAINBOW_OS_ANDROID)
                // Android doesn't ignore multiple '/' in paths.
                int i = -1;
                int j = -1;
                while (file[++i] != '\0')
                {
                    if (file[i] == '/' && file[i + 1] == '/')
                        continue;
                    path_[++j] = file[i];
                }
                path_[++j] = '\0';
#elif defined(RAINBOW_OS_IOS)
                NSString* string = [[NSString alloc]
                    initWithBytesNoCopy:(void*)file
                                 length:strlen(file)
                               encoding:NSUTF8StringEncoding
                           freeWhenDone:NO];
                string = [[NSBundle mainBundle]
                    pathForResource:[string stringByDeletingPathExtension]
                             ofType:[string pathExtension]];
                if (!string)
                    break;

                *this = [string UTF8String];
#else
                *this = ::g_current_path;
                *this += file;
#endif
            break;
        }
        case RelativeTo::UserDataPath: {
#ifdef RAINBOW_OS_IOS
                NSError* err = nil;
                NSString* libraryDir = [[[NSFileManager defaultManager]
                    URLForDirectory:NSLibraryDirectory
                           inDomain:NSUserDomainMask
                  appropriateForURL:nil
                             create:YES
                              error:&err] path];
                if (!libraryDir)
                    return;
                NSString* string = [[NSString alloc]
                    initWithBytesNoCopy:(void*)file
                                 length:strlen(file)
                               encoding:NSUTF8StringEncoding
                           freeWhenDone:NO];
                *this = [[libraryDir stringByAppendingPathComponent:string] UTF8String];
#else
                if (!g_user_data_path)
                    break;
                struct stat sb;
                if (stat(g_user_data_path, &sb) != 0)
                {
                    if (errno != ENOENT || create_directories(g_user_data_path, S_IRWXU | S_IRWXG) != 0)
                        break;
                }
                else if (!S_ISDIR(sb.st_mode))
                    break;
                *this = g_user_data_path;
                *this += file;
#endif  // RAINBOW_OS_IOS
            break;
        }
        case RelativeTo::Root:
            *this = file;
            break;
        default:
            break;
    }
}

#ifdef RAINBOW_TEST
int Path::create()
{
    return create_directories(path_, 0775);
}
#endif

#ifdef RAINBOW_OS_MACOS
CFURLRef Path::CreateCFURL() const
{
    CFStringRef str = CFStringCreateWithBytesNoCopy(
        kCFAllocatorDefault, reinterpret_cast<const UInt8*>(path_),
        strlen(path_), kCFStringEncodingUTF8, false, kCFAllocatorNull);
    CFURLRef url = CFURLCreateWithFileSystemPath(
        kCFAllocatorDefault, str, kCFURLPOSIXPathStyle, false);
    CFRelease(str);
    return url;
}
#endif

bool Path::is_file() const
{
    struct stat sb;
    return stat(path_, &sb) == 0 && S_ISREG(sb.st_mode);
}

Path& Path::operator=(const char* path)
{
    strncpy(path_, path, sizeof(path_) - sizeof(path_[0]));
    return *this;
}

Path& Path::operator+=(const char* path)
{
    const size_t last = strlen(path_) - 1;
    if (path_[last] != kPathSeparator)
        strcat(path_, kPathSeparatorLiteral);
    strcat(path_, path);
    return *this;
}

#ifdef RAINBOW_OS_IOS
Path::operator NSURL*() const
{
    NSString* path = [[NSString alloc]
        initWithBytesNoCopy:(void*)path_
                     length:strlen(path_)
                   encoding:NSUTF8StringEncoding
               freeWhenDone:NO];
    return [NSURL fileURLWithPath:path isDirectory:NO];
}
#endif
