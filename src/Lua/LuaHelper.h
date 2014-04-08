// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAHELPER_H_
#define LUA_LUAHELPER_H_

#include <new>

#include <lua.hpp>

#include "Common/Debug.h"
#include "Lua/LuaMacros.h"

#ifdef RAINBOW_OS_UNIX
#	define kTextBoldColorGreen  "\033[1;32m"
#	define kTextBoldColorRed    "\033[1;31m"
#	define kTextColorReset      "\033[0m"
#else
#	define kTextBoldColorGreen
#	define kTextBoldColorRed
#	define kTextColorReset
#endif

class Data;

NS_RAINBOW_LUA_BEGIN
{
	/// Creates a Lua wrapped object.
	template<class T>
	int alloc(lua_State *L);

	template<typename... Args>
	void argscheck(lua_State *L, int first, const char *syntax, Args&&... args);

	template<class T>
	int dealloc(lua_State *L);

	/// Outputs information about the error, and dumps the stack if applicable.
	void error(lua_State *L, const int lua_error);

	/// Custom Lua package loader.
	int load(lua_State *L);

	/// Loads buffer as a Lua chunk.
	/// \param chunk  Buffer to load.
	/// \param name   Name of the chunk. Used for debug information.
	/// \param exec   For internal use only! Whether to execute the loaded
	///               chunk. Only used by Lua package loaders.
	/// \return Number of successfully loaded chunks.
	int load(lua_State *L, const Data &chunk, const char *name, const bool exec = true);

	/// Pushes a collectable pointer on the stack.
	///
	/// Wraps pointer in a table so that one can attach an arbitrary metatable
	/// and have the garbage collector clean it up. Also sets the \c __type
	/// field for type checking.
	///
	/// \param ptr   The pointer to push on the stack.
	/// \param name  Name of the pointer type.
	void pushpointer(lua_State *L, void *ptr, const char *name);

	/// Simple C++-wrapper, loosely based on LunaWrapper.
	///
	/// Wraps a C++ object and makes its methods available in the namespace on
	/// top of the stack.
	///
	/// \see http://www.lua.org/manual/5.2/
	/// \see http://lua-users.org/wiki/LunaWrapper
	template<class T>
	void reg(lua_State *L);

	/// Reloads a previously loaded Lua chunk.
	/// \param chunk  Buffer to load.
	/// \param name   Name of the chunk. Used for debug information.
	/// \return Number of successfully reloaded chunks.
	int reload(lua_State *L, const Data &chunk, const char *name);

	/// Replaces the table at index \p n with its userdata if one exists.
	void replacetable(lua_State *L, const int n);

	/// Sets debugging hook.
	int sethook(lua_State *L, const int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE);

	/// Returns the pointer on top of the stack.
	///
	/// Unwraps the pointer while checking for nil value and type. This method
	/// may return a nullptr.
	///
	/// \see pushpointer()
	///
	/// \param name  Name of the pointer type to return.
	/// \return The pointer on the top of the stack if valid, else \c nullptr.
	void* topointer(lua_State *L, const char *name);

	/// Returns the string representing a Lua wrapped object. The format of the
	/// string is "<type name>: <address>". Normally only available for debug
	/// builds.
	template<class T>
	int tostring(lua_State *L);

	/// Returns the pointer returned from luaR_touserdata().
	template<class T>
	T* touserdata(lua_State *L, const int n);

	template<class T>
	int alloc(lua_State *L)
	{
		void *data = lua_newuserdata(L, sizeof(T));
		luaL_setmetatable(L, T::class_name);
		// Stash the userdata so we can return it later.
		const int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		new (data) T(L);
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
		return 1;
	}

	template<typename... Args>
#ifdef NDEBUG
	void argscheck(Args&&...) { }
#else
	void argscheck(lua_State *L, int first, const char *syntax, Args&&... args)
	{
		R_ASSERT(first > 0, "Invalid first argument number");

		static char error[512];

		const bool conditions[] { static_cast<bool>(args)... };
		int prev_index = first - 1;
		int token = 0;
		for (int i = 0; i < static_cast<int>(sizeof...(Args)); ++i)
		{
			if (!conditions[i])
			{
				for (; prev_index < i + first; ++prev_index, ++token)
				{
					while (syntax[token] != '(' && syntax[token] != ',')
					{
						error[token] = ' ';
						++token;
					}
				}
				--token;  // Undo the last increment.
				do
				{
					error[token] = ' ';
				} while (syntax[++token] == ' ');
				error[token] = '^';
				++token;
				while (syntax[token] != ')' && syntax[token] != ',')
				{
					error[token] = '~';
					++token;
				}
			}
		}
		if (prev_index)
		{
			error[token] = '\0';
			lua_Debug entry;
			lua_getstack(L, 0, &entry);
			lua_getinfo(L, "n", &entry);
			luaL_error(L,
			           kTextBoldColorRed "syntax:" kTextColorReset " bad argument(s) to '%s'\n"
			           "        %s\n"
			           "        " kTextBoldColorGreen "%s" kTextColorReset,
			           entry.name,
			           syntax,
			           error);
		}
	}
#endif

	template<class T>
	int dealloc(lua_State *L)
	{
		touserdata<T>(L, 1)->~T();
		return 0;
	}

	template<class T>
	void reg(lua_State *L)
	{
		if (T::is_constructible)
		{
			lua_pushstring(L, T::class_name);
			lua_pushcclosure(L, &alloc<T>, 0);
			lua_rawset(L, -3);
		}
		luaL_newmetatable(L, T::class_name);  // metatable = {}
		luaL_setfuncs(L, T::functions, 0);
		luaR_rawsetcclosurefield(L, &dealloc<T>, "__gc");
		luaR_rawsetcclosurefield(L, &tostring<T>, "__tostring");
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_rawset(L, -3);  // metatable.__index = metatable
		lua_pushliteral(L, "__metatable");
		lua_createtable(L, 0, 0);
		lua_rawset(L, -3);  // metatable.__metatable = {}
		lua_pop(L, 1);
	}

	template<class T>
	int tostring(lua_State *L)
	{
		lua_pushfstring(L, "%s: %p", T::class_name, touserdata<T>(L, 1));
		return 1;
	}

	template<class T>
	T* touserdata(lua_State *L, const int n)
	{
		return static_cast<T*>(luaR_touserdata(L, n, T::class_name));
	}
} NS_RAINBOW_LUA_END

#undef kTextBoldColorGreen
#undef kTextBoldColorRed
#undef kTextColorReset

#endif
