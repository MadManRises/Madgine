#pragma once

#include "scripting/datatypes/luatable.h"

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT LuaState
		{
		public:
			LuaState();
			virtual ~LuaState();

			static LuaState& getSingleton();

			void setFinalized();
			bool isFinalized() const;

			void executeString(lua_State* state, const std::string& cmd);

			LuaTable createThread();

			int pushThread(lua_State* state, lua_State* thread);

			LuaTable env() const;
			lua_State* state() const;

		private:
			bool mFinalized;

			lua_State* mState;
			LuaTable mRegistry;
			LuaTable mEnv;
			LuaTable mGlobal;


			std::map<lua_State *, int> mThreads;

			static LuaState* sSingleton;

			static const luaL_Reg sEnvMetafunctions[];
			static const luaL_Reg sScopeMetafunctions[];
			static const luaL_Reg sGlobalMetafunctions[];
			static const luaL_Reg sGlobalScopeMetafunctions[];

			static int lua_indexScope(lua_State* state);
			static int lua_pairsScope(lua_State* state);
			static int lua_nextScope(lua_State* state);
			static int lua_newindexGlobal(lua_State* state);
			static int lua_indexGlobalScope(lua_State* state);
			static int lua_indexEnv(lua_State* state);
			static int lua_pairsEnv(lua_State* state);
			static int lua_newindexEnv(lua_State* state);
			static int lua_tostringEnv(lua_State* state);
			static void pushGlobalScope(lua_State* state);
		};
	}
}
