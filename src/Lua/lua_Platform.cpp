// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Platform.h"

#include "Lua/LuaHelper.h"
#include "Platform/SystemInfo.h"

NS_RAINBOW_LUA_MODULE_BEGIN(platform)
{
    void init(lua_State* L)
    {
        lua_pushliteral(L, "platform");
        lua_createtable(L, 0, 4);

        luaR_rawsetboolean(
            L, "accelerometer", system_info::has_accelerometer());

        // Retrieve locale
        lua_pushliteral(L, "locale");
        {
            std::vector<std::unique_ptr<char[]>> locales;
            system_info::locales(locales);
            lua_createtable(L, locales.size(), 0);
            for (size_t i = 0; i < locales.size(); ++i)
            {
                lua_pushstring(L, locales[i].get());
                lua_rawseti(L, -2, i + 1);
            }
        }
        lua_rawset(L, -3);

        // Retrieve physical memory
        lua_pushliteral(L, "memory");
        lua_pushinteger(L, system_info::memory());
        lua_rawset(L, -3);

        // rainbow.platform.screen
        lua_pushliteral(L, "screen");
        lua_createtable(L, 0, 3);
        luaR_rawsetboolean(L, "touch", system_info::has_touchscreen());
        lua_rawset(L, -3);

        lua_rawset(L, -3);
    }

    void update(lua_State* L, const Vec2i& screen)
    {
        lua_getglobal(L, "rainbow");
        lua_pushliteral(L, "platform");
        lua_rawget(L, -2);
        lua_pushliteral(L, "screen");
        lua_rawget(L, -2);

        luaR_rawsetnumber(L, "width", screen.x);
        luaR_rawsetnumber(L, "height", screen.y);

        lua_pop(L, 3);
    }
} NS_RAINBOW_LUA_MODULE_END(platform)
