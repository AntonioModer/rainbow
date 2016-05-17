// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include <memory>

#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Lua/LuaHelper.h"

namespace rainbow
{
    Config::Config()
        : accelerometer_(true), high_dpi_(false), suspend_(true), width_(0),
          height_(0), msaa_(0)
    {
        const char kConfigModule[] = "config";

        const Path path(kConfigModule);
        if (!path.is_file())
        {
            LOGI("No config file was found");
            return;
        }

        const Data config(File::open(path));
        if (!config)
            return;

        auto L = lua::newstate();
        if (lua::load(L.get(), config, kConfigModule) == 0)
            return;

        lua_getglobal(L.get(), "accelerometer");
        if (lua_isboolean(L.get(), -1))
            accelerometer_ = lua_toboolean(L.get(), -1);

#ifdef RAINBOW_SDL
        lua_getglobal(L.get(), "allow_high_dpi");
        if (lua_isboolean(L.get(), -1))
            high_dpi_ = lua_toboolean(L.get(), -1);

        lua_getglobal(L.get(), "msaa");
        if (lua_isnumber(L.get(), -1))
        {
            msaa_ = lua::tointeger(L.get(), -1) &
                    (std::numeric_limits<decltype(msaa_)>::max() - 1);
            const auto msaa2 = next_pow2(msaa_);
            if (msaa2 != msaa_)
                msaa_ = msaa2 >> 1;
        }
#endif

        lua_getglobal(L.get(), "resolution");
        if (lua_istable(L.get(), -1))
        {
            lua_rawgeti(L.get(), -1, 1);
            width_ = lua::tointeger(L.get(), -1);
            lua_rawgeti(L.get(), -2, 2);
            height_ = lua::tointeger(L.get(), -1);
        }

#ifdef RAINBOW_SDL
        lua_getglobal(L.get(), "suspend_on_focus_lost");
        if (lua_isboolean(L.get(), -1))
            suspend_ = lua_toboolean(L.get(), -1);
#endif
    }
}
