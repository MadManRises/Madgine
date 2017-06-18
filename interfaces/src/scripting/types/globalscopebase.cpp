#include "interfaceslib.h"
#include "globalscopebase.h"

#include "scripting/parsing/scriptparser.h"

#include "scopebase.h"


extern "C" {
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */
}

namespace Engine {
namespace Scripting {

	const int GlobalScopeBase::sNoRef = LUA_NOREF;

	GlobalScopeBase::GlobalScopeBase(lua_State *state) :
	mState(state)
	{
		

	}

	bool Engine::Scripting::GlobalScopeBase::init()
	{
		lua_pushlightuserdata(mState, static_cast<ScopeBase*>(this));
		lua_setglobal(mState, "___scope___");

		lua_rawgeti(mState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		luaL_getmetatable(mState, "Interfaces.Metatable");

		lua_setmetatable(mState, -2);
		lua_pop(mState, 1);

		return ScopeBase::initGlobal(LUA_RIDX_GLOBALS);
	}

	void Engine::Scripting::GlobalScopeBase::finalize()
	{
		ScopeBase::finalize();
	}

	

	void GlobalScopeBase::addGlobal(ScopeBase * api)
	{
		assert(!api->getName().empty());
		mGlobals[api->getName()] = api;
	}

	void GlobalScopeBase::removeGlobal(ScopeBase * api)
	{
		mGlobals.erase(api->getName());
	}

	void GlobalScopeBase::pushScope(int table) {
		lua_rawgeti(mState, LUA_REGISTRYINDEX, table);
	}

	int GlobalScopeBase::registerScope(ScopeBase *scope) {

		lua_newtable(mState);

		lua_pushliteral(mState, "___scope___");
		lua_pushlightuserdata(mState, scope);

		lua_rawset(mState, -3);

		luaL_getmetatable(mState, "Interfaces.Metatable");

		lua_setmetatable(mState, -2);

		return luaL_ref(mState, LUA_REGISTRYINDEX);
	}

	void GlobalScopeBase::unregisterScope(int table) {
		luaL_unref(mState, LUA_REGISTRYINDEX, table);
	}

	bool GlobalScopeBase::hasMethod(ScopeBase *scope, const std::string &name) {
		lua_rawgeti(mState, LUA_REGISTRYINDEX, scope->table());

		lua_getfield(mState, -1, name.c_str());

		bool result = lua_isfunction(mState, -1);

		lua_pop(mState, 2);

		return result;
	}

	ArgumentList GlobalScopeBase::callMethod(ScopeBase *scope, const std::string &name, const ArgumentList &args) {
		int level = lua_gettop(mState);

		lua_rawgeti(mState, LUA_REGISTRYINDEX, scope->table());

		lua_getfield(mState, -1, name.c_str());

		if (!lua_isfunction(mState, -1)) {
			lua_pop(mState, 2);
			throw ScriptingException(Exceptions::unknownMethod(name, scope->getIdentifier()));
		}

		lua_insert(mState, -2);

		args.pushToStack(mState);

		switch (lua_pcall(mState, args.size() + 1, LUA_MULTRET, 0)) {
		case 0:
			break;
		case LUA_ERRRUN: {
			int iType = lua_type(mState, -1);
			const char *msg = "unknown Error";
			if (iType == LUA_TSTRING) {
				msg = lua_tostring(mState, -1);
			}
			else {
				lua_pop(mState, 1);
			}
			throw ScriptingException(std::string("Runtime Error: ") + msg);
		}
		case LUA_ERRMEM:
			throw ScriptingException("Lua Out-Of-Memory!");
		default:
			throw 0;
		}
		int nresults = lua_gettop(mState) - level;

		return ArgumentList(mState, nresults);
	}


	KeyValueMapList Engine::Scripting::GlobalScopeBase::maps()
	{
		return ScopeBase::maps().merge(mGlobals);
	}

}
}
