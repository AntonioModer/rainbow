// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_SceneGraph.h"

#include <unordered_set>

#include "Graphics/Drawable.h"
#include "Graphics/SceneGraph.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

using rainbow::SceneNode;

namespace
{
#ifndef NDEBUG
    std::unordered_set<SceneNode*> g_nodes;

    bool is_valid_node(lua_State* L, int n, SceneNode* node)
    {
        if (!node)
            return luaL_argerror(L, n, "invalid node");
        if (!g_nodes.count(node))
            return luaL_argerror(L, n, "non-existing node");
        return true;
    }

    bool register_node(SceneNode* node)
    {
        return g_nodes.insert(node).second;
    }

    bool unregister_node(SceneNode* node)
    {
        for_each(*node, [](SceneNode& node) { g_nodes.erase(&node); });
        return true;
    }
#endif

    template <typename T>
    T* unchecked_cast(lua_State* L, int n)
    {
        return static_cast<T*>(lua_touserdata(L, n));
    }

    SceneNode* tonode(lua_State* L, int n)
    {
        SceneNode* node = unchecked_cast<SceneNode>(L, n);
        R_ASSERT(is_valid_node(L, n, node), "Invalid or non-existing node");
        return node;
    }
}

NS_RAINBOW_LUA_BEGIN
{
    template <>
    const char SceneGraph::Bind::class_name[] = "scenegraph";

    template <>
    const bool SceneGraph::Bind::is_constructible = false;

    template <>
    const luaL_Reg SceneGraph::Bind::functions[] = {
        {"add_animation",   &SceneGraph::add_animation},
        {"add_batch",       &SceneGraph::add_batch},
        {"add_drawable",    &SceneGraph::add_drawable},
        {"add_label",       &SceneGraph::add_label},
        {"add_node",        &SceneGraph::add_node},
        {"attach_program",  &SceneGraph::attach_program},
        {"disable",         &SceneGraph::disable},
        {"enable",          &SceneGraph::enable},
        {"remove",          &SceneGraph::remove},
        {"set_parent",      &SceneGraph::set_parent},
        {"set_tag",         &SceneGraph::set_tag},
        {"move",            &SceneGraph::move},
        {nullptr,           nullptr}
    };

    SceneGraph* SceneGraph::create(lua_State* L, SceneNode* root)
    {
        R_ASSERT(register_node(root), "Failed to register root node");

        lua_pushlstring(L, class_name, strllen(class_name));
        void* data = lua_newuserdata(L, sizeof(SceneGraph));
        luaL_newmetatable(L, class_name);  // metatable = {}
        luaL_setfuncs(L, functions, 0);
        luaR_rawsetcfunction(L, "__tostring", &tostring<SceneGraph>);
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, -2);
        lua_rawset(L, -3);  // metatable.__index = metatable
        lua_pushliteral(L, "__metatable");
        lua_createtable(L, 0, 0);
        lua_rawset(L, -3);  // metatable.__metatable = {}
        lua_setmetatable(L, -2);
        lua_rawset(L, -3);

        return static_cast<SceneGraph*>(new (data) SceneGraph(root));
    }

#ifdef _MSC_VER
#   pragma warning(suppress: 4100)
#endif
    void SceneGraph::destroy(lua_State* L, SceneGraph* scenegraph)
    {
        lua_getglobal(L, "rainbow");
        lua_pushlstring(L, class_name, strllen(class_name));
        lua_pushnil(L);
        lua_rawset(L, -3);
        lua_pop(L, 1);
        scenegraph->~SceneGraph();
    }

    SceneGraph::SceneGraph(SceneNode* root) : node_(root) {}

    template <typename T, typename F>
    int SceneGraph::add_child(lua_State* L, F&& touserdata)
    {
        SceneGraph* self = Bind::self(L);
        if (!self)
            return 0;

        SceneNode* node;
        int obj;
        if (lua_isnoneornil(L, 3))
        {
            Argument<T>::is_required(L, 2);
            node = self->node_;
            obj = 2;
        }
        else
        {
            Argument<SceneNode>::is_required(L, 2);
            Argument<T>::is_required(L, 3);
            node = tonode(L, 2);
            obj = 3;
        }
        replacetable(L, obj);
        SceneNode* child = node->add_child(*touserdata(L, obj)->get());
        R_ASSERT(register_node(child), "Failed to register node");
        lua_pushlightuserdata(L, child);
        return 1;
    }

    int SceneGraph::add_animation(lua_State* L)
    {
        // rainbow.scenegraph:add_animation([node], <animation>)
        return add_child<Animation>(L, touserdata<Animation>);
    }

    int SceneGraph::add_batch(lua_State* L)
    {
        // rainbow.scenegraph:add_batch([node], <batch>)
        return add_child<SpriteBatch>(L, touserdata<SpriteBatch>);
    }

    int SceneGraph::add_drawable(lua_State* L)
    {
        // rainbow.scenegraph:add_drawable([node], <drawable>)
        return add_child<Drawable>(L, unchecked_cast<Drawable>);
    }

    int SceneGraph::add_label(lua_State* L)
    {
        // rainbow.scenegraph:add_label([node], <label>)
        return add_child<Label>(L, touserdata<Label>);
    }

    int SceneGraph::add_node(lua_State* L)
    {
        // rainbow.scenegraph:add_node([parent])
        Argument<SceneNode>::is_optional(L, 2);

        SceneGraph* self = Bind::self(L);
        if (!self)
            return 0;

        SceneNode* node = (lua_isuserdata(L, 2) ? tonode(L, 2) : self->node_);
        R_ASSERT(node != nullptr, "This shouldn't ever happen.");
        ASSUME(node != nullptr);

        auto child = SceneNode::create();
        R_ASSERT(register_node(child.get()), "Failed to register node");
        lua_pushlightuserdata(L, node->add_child(std::move(child)));
        return 1;
    }

    int SceneGraph::attach_program(lua_State* L)
    {
        // rainbow.scenegraph:attach_program(node, program)
        Argument<SceneNode>::is_required(L, 2);
        Argument<Shader>::is_required(L, 3);

        SceneGraph* self = Bind::self(L);
        if (!self)
            return 0;

        int program = (lua_isuserdata(L, 3) ? unchecked_cast<Shader>(L, 3)->id()
                                            : 0);
        tonode(L, 2)->attach_program(program);
        return 0;
    }

    int SceneGraph::disable(lua_State* L)
    {
        // rainbow.scenegraph:disable(node)
        Argument<SceneNode>::is_required(L, 2);

        tonode(L, 2)->set_enabled(false);
        return 0;
    }

    int SceneGraph::enable(lua_State* L)
    {
        // rainbow.scenegraph:enable(node)
        Argument<SceneNode>::is_required(L, 2);

        tonode(L, 2)->set_enabled(true);
        return 0;
    }

    int SceneGraph::remove(lua_State* L)
    {
        // rainbow.scenegraph:remove(node)
        Argument<SceneNode>::is_required(L, 2);

        SceneNode* node = tonode(L, 2);
        R_ASSERT(unregister_node(node), "Failed to unregister node");
        node->remove();
        return 0;
    }

    int SceneGraph::set_parent(lua_State* L)
    {
        // rainbow.scenegraph:set_parent(parent, child)
        Argument<SceneNode>::is_required(L, 2);
        Argument<SceneNode>::is_required(L, 3);

        tonode(L, 2)->add_child(tonode(L, 3));
        return 0;
    }

    int SceneGraph::set_tag(lua_State* L)
    {
        // rainbow.scenegraph:set_tag(node, tag)
        Argument<SceneNode>::is_required(L, 2);
        Argument<char*>::is_required(L, 3);

#if USE_NODE_TAGS
        tonode(L, 2)->set_tag(lua_tostring(L, 3));
#endif
        return 0;
    }

    int SceneGraph::move(lua_State* L)
    {
        // rainbow.scenegraph:move(node, x, y)
        Argument<SceneNode>::is_required(L, 2);
        Argument<lua_Number>::is_required(L, 3);
        Argument<lua_Number>::is_required(L, 4);

        const Vec2f delta(lua_tonumber(L, 3), lua_tonumber(L, 4));
        if (delta.is_zero())
            return 0;

        tonode(L, 2)->move(delta);
        return 0;
    }

    template <>
    const char ScopedNode::Bind::class_name[] = "scoped_node";

    template <>
    const bool ScopedNode::Bind::is_constructible = true;

    template <>
    const luaL_Reg ScopedNode::Bind::functions[] = {{nullptr, nullptr}};

    ScopedNode::ScopedNode(lua_State* L) : node_(nullptr), state_(L)
    {
        Argument<SceneNode>::is_required(L, 1);
        node_ = tonode(L, 1);
    }

    ScopedNode::~ScopedNode()
    {
        if (!node_)
            return;

        lua_getglobal(state_, "rainbow");
        lua_pushlstring(
            state_, SceneGraph::class_name, strllen(SceneGraph::class_name));
        lua_gettable(state_, -2);
        const bool is_shutting_down = lua_isnil(state_, -1);
        lua_pop(state_, 2);
        if (is_shutting_down)
            return;

        node_->remove();
    }
} NS_RAINBOW_LUA_END
