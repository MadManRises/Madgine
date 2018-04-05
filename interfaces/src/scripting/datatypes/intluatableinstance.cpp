#include "interfaceslib.h"
#include "intluatableinstance.h"

extern "C"
{
#include <lua/lua.h>                                /* Always include this when calling Lua */
#include <lua/lauxlib.h>                            /* Always include this when calling Lua */
}

namespace Engine
{
	namespace Scripting
	{

		IntLuaTableInstance::IntLuaTableInstance(lua_State* state, int index,
		                                         const std::shared_ptr<LuaTableInstance>& parent) :
			LuaTableInstance(state, parent),
			mIndex(index)
		{
		}

		IntLuaTableInstance::~IntLuaTableInstance()
		{
			if (mIndex != LUA_REGISTRYINDEX)
			{
				if (mParent)
				{
					mParent->push(mState);
					luaL_unref(mState, -1, mIndex);
					lua_pop(mState, 1);
				}
				else if (mIndex != LUA_RIDX_GLOBALS)
				{
					luaL_unref(mState, LUA_REGISTRYINDEX, mIndex);
				}
			}
		}

		
		void IntLuaTableInstance::push(lua_State* state) const
		{
			if (mIndex == LUA_REGISTRYINDEX)
			{
				lua_pushvalue(state ? state : mState, LUA_REGISTRYINDEX);
			}
			else if (mParent)
			{
				mParent->push(state ? state : mState);
				lua_rawgeti(state ? state : mState, -1, mIndex);
				lua_replace(state ? state : mState, -2);
			}
			else
			{
				lua_rawgeti(state ? state : mState, LUA_REGISTRYINDEX, mIndex);
			}
		}


	}
}
