#include "interfaceslib.h"
#include "stringluatableinstance.h"
#include "argumentlist.h"
#include "luatableiterator.h"

extern "C"
{
#include <lua/lua.h>                                /* Always include this when calling Lua */
#include <lua/lauxlib.h>                            /* Always include this when calling Lua */
}

namespace Engine
{
	namespace Scripting
	{
		
		StringLuaTableInstance::StringLuaTableInstance(lua_State* state, const std::string& index,
		                                               const std::shared_ptr<LuaTableInstance>& parent) :
			LuaTableInstance(state, parent),
			mIndex(index)
		{
		}

		StringLuaTableInstance::~StringLuaTableInstance()
		{
			if (mParent)
			{
				mParent->push(mState);
				lua_pushnil(mState);
				lua_setfield(mState, -2, mIndex.c_str());
				lua_pop(mState, 1);
			}
			else
			{
				lua_pushnil(mState);
				lua_setfield(mState, LUA_REGISTRYINDEX, mIndex.c_str());
			}
		}




		void StringLuaTableInstance::push(lua_State* state) const
		{
			if (mParent)
			{
				mParent->push(state ? state : mState);
				lua_getfield(state ? state : mState, -1, mIndex.c_str());
				lua_replace(state ? state : mState, -2);
			}
			else
			{
				lua_getfield(state ? state : mState, LUA_REGISTRYINDEX, mIndex.c_str());
			}
		}




	}
}
