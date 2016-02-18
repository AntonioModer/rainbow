// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/SharedLibrary.h"

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_WINDOWS
#   include <Windows.h>
#else
#   include <dlfcn.h>
#endif

using rainbow::SharedLibrary;

SharedLibrary::SharedLibrary(const char* path)
{
#ifdef RAINBOW_OS_WINDOWS
    handle_ = LoadLibrary(path);
#else
    handle_ = dlopen(path, RTLD_NOW | RTLD_LOCAL);
#endif
}

SharedLibrary::~SharedLibrary()
{
    if (handle_ == nullptr)
        return;

#ifdef RAINBOW_OS_WINDOWS
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif
}

auto SharedLibrary::get_symbol(const char* symbol) const -> void*
{
    return
#ifdef RAINBOW_OS_WINDOWS
        GetProcAddress(static_cast<HMODULE>(handle_), symbol);
#else
        dlsym(handle_, symbol);
#endif
}
