// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Input.h"

#include "Input/Input.h"
#include "Input/Pointer.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

using rainbow::ControllerAxis;
using rainbow::ControllerButton;
using rainbow::KeyStroke;
using rainbow::VirtualKey;

NS_RAINBOW_LUA_MODULE_BEGIN(input)
{
    namespace
    {
        enum Event
        {
            kEventKeyDown,
            kEventKeyUp,
            kEventPointerBegan,
            kEventPointerCanceled,
            kEventPointerEnded,
            kEventPointerMoved
        };

        int acceleration = -1;
        int events = -1;
        size_t event_count = std::numeric_limits<size_t>::max();

        const char* const kInputEvents[]{"key_down",
                                         "key_up",
                                         "pointer_began",
                                         "pointer_canceled",
                                         "pointer_ended",
                                         "pointer_moved"};

        int axis(lua_State* L)
        {
            // rainbow.input.axis(controller, axis)
            Argument<lua_Number>::is_required(L, 1);
            Argument<lua_Number>::is_required(L, 2);

            const auto controller = static_cast<unsigned>(lua_tointeger(L, 1));
            const auto axis = static_cast<unsigned>(lua_tointeger(L, 2));
            lua_pushboolean(
                L,
                controller < Input::kNumSupportedControllers &&
                    axis < to_underlying_type(ControllerAxis::Count) &&
                    Input::Get()->axis(
                        controller, static_cast<ControllerAxis>(axis)));
            return 1;
        }

        int is_button_down(lua_State* L)
        {
            // rainbow.input.is_button_down(controller, button)
            Argument<lua_Number>::is_required(L, 1);
            Argument<lua_Number>::is_required(L, 2);

            const auto controller = static_cast<unsigned>(lua_tointeger(L, 1));
            const auto button = static_cast<unsigned>(lua_tointeger(L, 2));
            lua_pushboolean(
                L,
                controller < Input::kNumSupportedControllers &&
                    button < to_underlying_type(ControllerButton::Count) &&
                    Input::Get()->is_down(
                        controller, static_cast<ControllerButton>(button)));
            return 1;
        }

        int is_key_down(lua_State* L)
        {
            // rainbow.input.is_key_down(key)
            Argument<lua_Number>::is_required(L, 1);

            const unsigned key = static_cast<unsigned>(lua_tointeger(L, 1));
            lua_pushboolean(
                L,
                key < to_underlying_type(VirtualKey::KeyCount) &&
                    Input::Get()->is_down(static_cast<VirtualKey>(key)));
            return 1;
        }

        void push_event(lua_State* L, Event event)
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, events);
            lua_pushinteger(L, ++event_count);
            lua_rawseti(L, -2, 0);

            lua_rawgeti(L, -1, event_count);
            if (lua_type(L, -1) != LUA_TTABLE)
            {
                lua_pop(L, 1);
                lua_createtable(L, 2, 0);
                lua_pushvalue(L, -1);
                lua_rawseti(L, -3, event_count);
            }
            lua_pushstring(L, kInputEvents[event]);
            lua_rawseti(L, -2, 1);
        }

        void on_key_event(lua_State* L, Event event, const KeyStroke& key)
        {
            push_event(L, event);

            lua_createtable(L, 2, 0);
            lua_pushinteger(L, static_cast<lua_Integer>(key.key));
            lua_rawseti(L, -2, 1);
            lua_pushinteger(L, key.mods);
            lua_rawseti(L, -2, 2);

            lua_rawseti(L, -2, 2);
            lua_pop(L, 2);
        }

        void on_pointer_event(lua_State* L,
                              Event event,
                              const ArrayView<Pointer>& pointers)
        {
            push_event(L, event);

            if (pointers.size() == 0)
                lua_pushnil(L);
            else
            {
                lua_createtable(L, 0, pointers.size());
                for (auto&& p : pointers)
                {
                    lua_pushinteger(L, p.hash);
                    lua_createtable(L, 0, 3);
                    luaR_rawsetinteger(L, "x", p.x);
                    luaR_rawsetinteger(L, "y", p.y);
                    luaR_rawsetinteger(L, "timestamp", p.timestamp);
                    lua_rawset(L, -3);
                }
            }

            lua_rawseti(L, -2, 2);
            lua_pop(L, 2);
        }
    }

    void init(lua_State* L)
    {
        lua_pushliteral(L, "input");
        lua_createtable(L, 0, 14);

        // rainbow.input.acceleration
        lua_pushliteral(L, "acceleration");
        lua_createtable(L, 0, 4);
        lua_pushvalue(L, -1);
        acceleration = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_rawset(L, -3);

        // rainbow.input.axis
        luaR_rawsetcfunction(L, "axis", &axis);

        // rainbow.input.is_button_down
        luaR_rawsetcfunction(L, "is_button_down", &is_button_down);

        // rainbow.input.is_key_down
        luaR_rawsetcfunction(L, "is_key_down", &is_key_down);

        lua_pushliteral(L, "__events");
        lua_createtable(L, 0, 0);
        lua_pushvalue(L, -1);
        events = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_rawset(L, -3);

        lua_rawset(L, -3);
        clear(L);
    }

    void accelerated(lua_State* L, const Acceleration& a)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, acceleration);
        luaR_rawsetnumber(L, "x", a.x());
        luaR_rawsetnumber(L, "y", a.y());
        luaR_rawsetnumber(L, "z", a.z());
        luaR_rawsetnumber(L, "timestamp", a.timestamp());
        lua_pop(L, 1);
    }

    void clear(lua_State* L)
    {
        if (event_count == 0)
            return;

        lua_rawgeti(L, LUA_REGISTRYINDEX, events);
        lua_pushinteger(L, 0);
        lua_rawseti(L, -2, 0);
        lua_pop(L, 1);
        event_count = 0;
    }

    void on_key_down(lua_State* L, const KeyStroke& key)
    {
        on_key_event(L, kEventKeyDown, key);
    }

    void on_key_up(lua_State* L, const KeyStroke& key)
    {
        on_key_event(L, kEventKeyUp, key);
    }

    void on_pointer_began(lua_State* L, const ArrayView<Pointer>& pointers)
    {
        on_pointer_event(L, kEventPointerBegan, pointers);
    }

    void on_pointer_canceled(lua_State* L)
    {
        on_pointer_event(L, kEventPointerCanceled, nullptr);
    }

    void on_pointer_ended(lua_State* L, const ArrayView<Pointer>& pointers)
    {
        on_pointer_event(L, kEventPointerEnded, pointers);
    }

    void on_pointer_moved(lua_State* L, const ArrayView<Pointer>& pointers)
    {
        on_pointer_event(L, kEventPointerMoved, pointers);
    }
} NS_RAINBOW_LUA_MODULE_END(input)
