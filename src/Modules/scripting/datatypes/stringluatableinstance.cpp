#include "../../moduleslib.h"
#include "stringluatableinstance.h"
#include "luatableiterator.h"

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
}

#include "luathread.h"

namespace Engine
{
	namespace Scripting
	{
		
		StringLuaTableInstance::StringLuaTableInstance(LuaThread* thread, const std::string& index,
		                                               const std::shared_ptr<LuaTableInstance>& parent) :
			LuaTableInstance(thread, parent),
			mIndex(index)
		{
		}

		StringLuaTableInstance::~StringLuaTableInstance()
		{
			std::lock_guard guard(*mThread);
			lua_State *state = mThread->state();
			if (mParent)
			{
				mParent->push(state);
				lua_pushnil(state);
				lua_setfield(state, -2, mIndex.c_str());
				lua_pop(state, 1);
			}
			else
			{
				lua_pushnil(state);
				lua_setfield(state, LUA_REGISTRYINDEX, mIndex.c_str());
			}
		}




		void StringLuaTableInstance::push(lua_State* state) const
		{
			if (mParent)
			{
				mParent->push(state);
				lua_getfield(state, -1, mIndex.c_str());
				lua_replace(state, -2);
			}
			else
			{
				lua_getfield(state, LUA_REGISTRYINDEX, mIndex.c_str());
			}
		}




	}
}
