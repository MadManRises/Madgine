#pragma once


extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
#include "../../lua/lualib.h"
}

#include "luatable.h"

namespace Engine
{
	namespace Scripting
	{
		struct LuaThread
		{

			LuaThread() :
				mScope(nullptr),
				mState(luaL_newstate()),
				mIndex(LUA_NOREF),
				mTable()
			{
			}

			LuaThread(LuaTable &registry, ScopeBase *scope) :
				mScope(scope),
				mState(lua_newthread(registry.thread()->state())),
				mIndex(luaL_ref(registry.thread()->state(), LUA_REGISTRYINDEX)),
				mTable(registry.createTable(this))
			{
			}

			LuaThread(LuaThread &&other) = delete;

			~LuaThread()
			{
				if (mIndex == LUA_NOREF)
				{
					lua_close(mState);
				}
				else
				{
					luaL_unref(mState, LUA_REGISTRYINDEX, mIndex);
				}
			}

			bool operator==(const LuaThread& other) const
			{
				return other.mState == mState;
			}

			lua_State *state() const
			{
				return mState;
			}

			int push(lua_State *state) const
			{
				lua_rawgeti(state, LUA_REGISTRYINDEX, mIndex);
				return 1;
			}

			LuaTable table() const
			{
				return mTable;
			}

			ScopeBase *scope() const
			{
				return mScope;
			}

			void lock()
			{
				mMutex.lock();
			}

			void unlock()
			{
				mMutex.unlock();
			}

		private:
			mutable std::recursive_mutex mMutex;
			ScopeBase *mScope;
			lua_State* mState;
			int mIndex;
			LuaTable mTable;				
		};

	}
}