#include "../../interfaceslib.h"
#include "intluatableinstance.h"

#include "luathread.h"

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
}

namespace Engine
{
	namespace Scripting
	{

		IntLuaTableInstance::IntLuaTableInstance(LuaThread* thread, int index,
		                                         const std::shared_ptr<LuaTableInstance>& parent) :
			LuaTableInstance(thread, parent),
			mIndex(index)
		{
		}

		IntLuaTableInstance::~IntLuaTableInstance()
		{
			if (mIndex != LUA_REGISTRYINDEX && mIndex != LUA_RIDX_GLOBALS)
			{
				std::lock_guard guard(*mThread);
				lua_State *state = mThread->state();
				if (mParent)
				{
					mParent->push(state);
					luaL_unref(state, -1, mIndex);
					lua_pop(state, 1);
				}
				else if (mIndex != LUA_RIDX_GLOBALS)
				{
					luaL_unref(state, LUA_REGISTRYINDEX, mIndex);
				}				
			}
		}

		
		void IntLuaTableInstance::push(lua_State* state) const
		{
			if (mIndex == LUA_REGISTRYINDEX)
			{
				lua_pushvalue(state, LUA_REGISTRYINDEX);
			}
			else if (mParent)
			{
				mParent->push(state);
				lua_rawgeti(state, -1, mIndex);
				lua_replace(state, -2);
			}
			else
			{
				lua_rawgeti(state, LUA_REGISTRYINDEX, mIndex);
			}
		}


	}
}
