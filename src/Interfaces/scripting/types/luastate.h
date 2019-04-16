#pragma once

#include "../datatypes/luatable.h"
#include "../datatypes/luathread.h"

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT LuaState
		{
		public:
			static LuaState &getSingleton();

			LuaState();
			virtual ~LuaState();


			void setFinalized();
			bool isFinalized() const;

			void executeString(lua_State* state, const std::string& cmd);

			const LuaThread *createThread(ScopeBase *global);
			static const LuaThread *getThread(lua_State *thread);

			int pushThread(lua_State* state, lua_State* thread);

			LuaTable env() const;
			LuaThread* mainThread();

			void setGlobalMethod(const std::string &name, int (*f)(lua_State*));			

		private:
			bool mFinalized;

			LuaTable mRegistry;
			LuaTable mEnv;
			LuaTable mGlobal;

			LuaThread mMainThread;

			struct LuaThreadComparator
			{
				using is_transparent = void;

				bool operator()(const LuaThread &first, const LuaThread &second) const
				{
					return first.state() < second.state();
				}

				bool operator()(const LuaThread &first, lua_State *second) const
				{
					return first.state() < second;
				}

				bool operator()(lua_State *first, const LuaThread &second) const
				{
					return first < second.state();
				}
			};
			std::set<LuaThread, LuaThreadComparator> mThreads;

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
		};
	}
}
