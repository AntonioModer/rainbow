// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Texture.h"

#include <lua.hpp>

#include "FileSystem/Path.h"

NS_RAINBOW_LUA_BEGIN
{
    template <>
    const char Texture::Bind::class_name[] = "texture";

    template <>
    const bool Texture::Bind::is_constructible = true;

    template <>
    const luaL_Reg Texture::Bind::functions[]{
        {"create",  &Texture::create},
        {"trim",    &Texture::trim},
        {nullptr,   nullptr}};

    Texture::Texture(lua_State* L)
    {
        // rainbow.texture("/path/to/texture")
        Argument<char*>::is_required(L, 1);

        texture_ = make_shared<TextureAtlas>(lua_tostring(L, 1));
        if (!texture_->is_valid())
            luaL_error(L, "rainbow.texture: Failed to create texture");
    }

    SharedPtr<TextureAtlas> Texture::get() const { return texture_; }

    int Texture::create(lua_State* L)
    {
        // <texture>:create(x, y, width, height)
        Argument<lua_Number>::is_required(L, 2);
        Argument<lua_Number>::is_required(L, 3);
        Argument<lua_Number>::is_required(L, 4);
        Argument<lua_Number>::is_required(L, 5);

        Texture* self = Bind::self(L);
        if (!self)
            return 0;

        const int x = lua_tointeger(L, 2);
        const int y = lua_tointeger(L, 3);
        const int w = lua_tointeger(L, 4);
        const int h = lua_tointeger(L, 5);
        lua_pushinteger(L, self->texture_->add_region(x, y, w, h));
        return 1;
    }

    int Texture::trim(lua_State* L)
    {
        Texture* self = Bind::self(L);
        if (!self)
            return 0;

        self->texture_->trim();
        return 0;
    }
} NS_RAINBOW_LUA_END
