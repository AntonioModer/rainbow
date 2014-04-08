// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Sprite.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Sprite::Bind::class_name[] = "sprite";

	template<>
	const bool Sprite::Bind::is_constructible = false;

	template<>
	const luaL_Reg Sprite::Bind::functions[] = {
		{ "get_angle",     &Sprite::get_angle },
		{ "get_color",     &Sprite::get_color },
		{ "get_position",  &Sprite::get_position },
		{ "set_color",     &Sprite::set_color },
		{ "set_normal",    &Sprite::set_normal },
		{ "set_pivot",     &Sprite::set_pivot },
		{ "set_position",  &Sprite::set_position },
		{ "set_rotation",  &Sprite::set_rotation },
		{ "set_scale",     &Sprite::set_scale },
		{ "set_texture",   &Sprite::set_texture },
		{ "mirror",        &Sprite::mirror },
		{ "move",          &Sprite::move },
		{ "rotate",        &Sprite::rotate },
		{ nullptr, nullptr }
	};

	Sprite::Sprite(lua_State *L)
	    : id(lua_tointeger(L, 2)),
	      batch(static_cast<SpriteBatch*>(lua_touserdata(L, 1))) { }

	int Sprite::get_angle(lua_State *L)
	{
		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushnumber(L, self->sprite().angle());
		return 1;
	}

	int Sprite::get_color(lua_State *L)
	{
		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		const Colorb& c = self->sprite().color();
		lua_pushinteger(L, c.r);
		lua_pushinteger(L, c.g);
		lua_pushinteger(L, c.b);
		lua_pushinteger(L, c.a);
		return 4;
	}

	int Sprite::get_position(lua_State *L)
	{
		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		const Vec2f &v = self->sprite().position();
		lua_pushnumber(L, v.x);
		lua_pushnumber(L, v.y);
		return 2;
	}

	int Sprite::set_color(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_color(r, g, b, a = 255)",
		          lua_isnumber(L, 2),
		          lua_isnumber(L, 3),
		          lua_isnumber(L, 4),
		          lua_isnumber(L, 5));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		unsigned int color = lua_tointeger(L, 2) << 24;
		color += lua_tointeger(L, 3) << 16;
		color += lua_tointeger(L, 4) << 8;
		color += luaR_optinteger(L, 5, 0xff);
		self->sprite().set_color(color);
		return 0;
	}

	int Sprite::set_normal(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_normal(<texture>)", lua_isnumber(L, 2));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().set_normal(lua_tointeger(L, 2));
		return 0;
	}

	int Sprite::set_pivot(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_pivot(x, y)",
		          lua_isnumber(L, 2), lua_isnumber(L, 3));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().set_pivot(Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}

	int Sprite::set_position(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_position(x, y)",
		          lua_isnumber(L, 2), lua_isnumber(L, 3));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().set_position(
		    Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}

	int Sprite::set_rotation(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_rotation(r)", lua_isnumber(L, 2));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().set_rotation(lua_tonumber(L, 2));
		return 0;
	}

	int Sprite::set_scale(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_scale(fx, fy = fx)",
		          lua_isnumber(L, 2),
		          (lua_isnumber(L, 3) || lua_isnone(L, 3)));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		const float fx = lua_tonumber(L, 2);
		self->sprite().set_scale(Vec2f(fx, luaR_optnumber(L, 3, fx)));
		return 0;
	}

	int Sprite::set_texture(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:set_texture(<texture>)", lua_isnumber(L, 2));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().set_texture(lua_tointeger(L, 2));
		return 0;
	}

	int Sprite::mirror(lua_State *L)
	{
		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().mirror();
		return 0;
	}

	int Sprite::move(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:move(x, y)",
		          lua_isnumber(L, 2), lua_isnumber(L, 3));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().move(Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}

	int Sprite::rotate(lua_State *L)
	{
		argscheck(L, 2, "<sprite>:rotate(r)", lua_isnumber(L, 2));

		Sprite *self = Bind::self(L);
		if (!self)
			return 0;

		self->sprite().rotate(lua_tonumber(L, 2));
		return 0;
	}

	::Sprite& Sprite::sprite() const
	{
		return this->batch->sprites()[this->id];
	}
} NS_RAINBOW_LUA_END
