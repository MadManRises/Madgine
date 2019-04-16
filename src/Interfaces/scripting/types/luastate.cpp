#include "../../interfaceslib.h"

#include "luastate.h"

#include "../../generic/keyvalueiterate.h"

#include "scopebase.h"
#include "apihelper.h"

#include "../datatypes/luatablefieldaccessor.h"

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
#include "../../lua/lualib.h"
}

namespace Engine
{
	namespace Scripting
	{
		LuaState* LuaState::sSingleton = nullptr;

		const luaL_Reg LuaState::sScopeMetafunctions[] =
		{
			{"__index", &lua_indexScope},
			{"__pairs", &lua_pairsScope},
			{nullptr, nullptr}
		};

		const luaL_Reg LuaState::sGlobalMetafunctions[] =
		{
			{"__newindex", &lua_newindexGlobal},
			{nullptr, nullptr}
		};

		const luaL_Reg LuaState::sEnvMetafunctions[] =
		{
			{"__index", &lua_indexEnv},
			{"__newindex", &lua_newindexEnv},
			{"__pairs", &lua_pairsEnv},
			{"__tostring", &lua_tostringEnv},
			{nullptr, nullptr}
		};

		const luaL_Reg LuaState::sGlobalScopeMetafunctions[] =
		{
			{"__index", &lua_indexGlobalScope},
			{"__pairs", &lua_pairsScope},
			{nullptr, nullptr}
		};


		LuaState& LuaState::getSingleton()
		{
			assert(sSingleton);
			return *sSingleton;
		}

		LuaState::LuaState() :
			mFinalized(false)
		{
			assert(!sSingleton);
			sSingleton = this;

			mRegistry = LuaTable::registry(&mMainThread);

			lua_State *state = mMainThread.state();

			luaL_openlibs(state);

			luaL_newmetatable(state, "Interfaces.Scope");
			luaL_setfuncs(state, sScopeMetafunctions, 0);
			lua_pop(state, 1);

			luaL_newmetatable(state, "Interfaces.Global");
			luaL_setfuncs(state, sGlobalMetafunctions, 0);
			lua_pop(state, 1);

			luaL_newmetatable(state, "Interfaces.Env");
			luaL_setfuncs(state, sEnvMetafunctions, 0);
			lua_pop(state, 1);

			luaL_newmetatable(state, "Interfaces.GlobalScope");
			luaL_setfuncs(state, sGlobalScopeMetafunctions, 0);
			lua_pop(state, 1);

			APIHelper::createMetatables(state);

			mEnv = mRegistry.createTable();

			LuaTable tempMetatable = mRegistry.createTable();

			mGlobal = LuaTable::global(&mMainThread);

			tempMetatable["__index"] = ValueType(mGlobal);
			tempMetatable["__newindex"] = ValueType(mGlobal);

			mEnv.setMetatable(tempMetatable);
		}

		LuaState::~LuaState()
		{
			mGlobal.clear();
			mRegistry.clear();
			mEnv.clear();
			mThreads.clear();

			sSingleton = nullptr;
		}


		void LuaState::setFinalized()
		{
			mGlobal.setMetatable("Interfaces.Global");
			mEnv.setMetatable("Interfaces.Env");

			mFinalized = true;
		}

		bool LuaState::isFinalized() const
		{
			return mFinalized;
		}


		void LuaState::executeString(lua_State* state, const std::string& cmd)
		{
			if (luaL_loadstring(state, cmd.c_str()))
			{
				throw ScriptingException(lua_tostring(state, -1));
			}
			mEnv.push(state);
			lua_setupvalue(state, -2, 1);

			if (lua_pcall(state, 0, LUA_MULTRET, 0))
			{
				throw ScriptingException(lua_tostring(state, -1));
			}
		}


		const LuaThread *LuaState::createThread(ScopeBase *global)
		{
			std::lock_guard guard(mMainThread);

			auto pib = mThreads.emplace(mRegistry, global);
			assert(pib.second);

			return &*pib.first;
		}

		const LuaThread * LuaState::getThread(lua_State * thread)
		{
			return &*sSingleton->mThreads.find(thread);
		}

		int LuaState::pushThread(lua_State* state, lua_State* thread)
		{
			return mThreads.find(thread)->push(state);			
		}

		LuaTable LuaState::env() const
		{
			return mEnv;
		}

		LuaThread* LuaState::mainThread()
		{
			return &mMainThread;
		}

		void LuaState::setGlobalMethod(const std::string& name, int(* f)(lua_State*))
		{
			assert(!mFinalized);

			std::lock_guard guard(mMainThread);
			lua_State *state = mMainThread.state();

			mGlobal.push(state);
			lua_pushcfunction(state, f);
			lua_setfield(state, -2, name.c_str());
			lua_pop(state, 1);
		}

		int LuaState::lua_indexScope(lua_State* state)
		{
			ScopeBase* scope = ValueType::fromStack(state, -2).as<ScopeBase*>();

			std::string key = lua_tostring(state, -1);

			lua_pop(state, 2);

			std::optional<ValueType> v = scope->get(key);
			if (!v)
				return 0;
			v->push(state);
			return 1;
		}

		int LuaState::lua_newindexGlobal(lua_State* state)
		{
			return luaL_error(state, "Trying to modify read-only Global");
		}

		int LuaState::lua_indexGlobalScope(lua_State* state)
		{
			ScopeBase* scope = ValueType::fromStack(state, -2).as<ScopeBase*>();

			std::string key = lua_tostring(state, -1);

			lua_pop(state, 2);

			std::optional<ValueType> v = scope->get(key);
			if (v)
			{
				v->push(state);
				return 1;
			}

			lua_getglobal(state, key.c_str());
			return 1;
		}

		int LuaState::lua_pairsScope(lua_State* state)
		{
			if (lua_gettop(state) != 1 || !lua_istable(state, -1))
				luaL_error(state, "illegal Call to __pairs!");

			lua_pushcfunction(state, &lua_nextScope);

			lua_insert(state, -2);

			return 2;
		}

		int LuaState::lua_nextScope(lua_State* state)
		{
			lua_settop(state, 2);

			ScopeBase* scope = ValueType::fromStack(state, -2).as<ScopeBase*>();

			assert(scope);

			std::shared_ptr<KeyValueIterator> it;

			if (lua_isnil(state, -1))
			{
				lua_pop(state, 1);
				it = scope->iterator();
				ValueType(it).push(state);
			}
			else if (lua_isuserdata(state, -1))
			{
				it = ValueType::fromStack(state, -1).as<std::shared_ptr<KeyValueIterator>>();
				++*it;
			}

			if (it)
			{
				if (!it->ended())
				{
					return 1 + it->value().push(state);
				}
				lua_pop(state, 1);
				lua_pushnil(state);
			}

			lua_getglobal(state, "next");
			lua_insert(state, -3);
			lua_call(state, 2, 2);
			return 2;
		}

		int LuaState::lua_newindexEnv(lua_State* state)
		{
			getThread(state)->table().push(state);
			lua_replace(state, -4);
			lua_settable(state, -3);
			return 1;
		}


		/*void stackdump_g(lua_State* l)
		{
			int i;
			int top = lua_gettop(l);

			std::cout << "total in stack " << top << std::endl;

			for (i = 1; i <= top; i++)
			{  // repeat for each level 
				int t = lua_type(l, i);
				switch (t) {
				case LUA_TSTRING:  // strings 
					std::cout << "string: '" << lua_tostring(l, i) << "'" << std::endl;
					break;
				case LUA_TBOOLEAN:  // booleans 
					std::cout << "boolean " << (lua_toboolean(l, i) ? "true" : "false") << std::endl;
					break;
				case LUA_TNUMBER:  // numbers 
					std::cout << "number: " << lua_tonumber(l, i) << std::endl;
					break;
				default:  // other values 
					std::cout << lua_typename(l, t) << std::endl;;
					break;
				}
			}
		}*/

		int LuaState::lua_indexEnv(lua_State* state)
		{
			getThread(state)->table().push(state);
			lua_replace(state, -3);
			lua_gettable(state, -2);
			lua_replace(state, -2);
			return 1;
		}

		int LuaState::lua_pairsEnv(lua_State* state)
		{
			lua_pop(state, 1);
			lua_pushcfunction(state, &lua_nextScope);
			getThread(state)->table().push(state);
			return 2;
		}

		int LuaState::lua_tostringEnv(lua_State* state)
		{
			getThread(state)->table().push(state);
			lua_replace(state, -2);
			luaL_tolstring(state, -1, nullptr);
			return 1;
		}

	}
}
