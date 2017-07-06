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

	GlobalScopeBase::GlobalScopeBase(const std::pair<lua_State *, int> &state) :
	mState(state.first),
	mTable(state.second)
	{
		

	}

	bool GlobalScopeBase::init()
	{
		if (!mState) {
			std::tie(mState, mTable) = Engine::Scripting::Parsing::ScriptParser::getSingleton().createThread();
		}

		lua_rawgeti(mState, LUA_REGISTRYINDEX, mTable);
		
		lua_newtable(mState);
		lua_setfield(mState, -2, "__scopes__");
	
		lua_pop(mState, 1);

		return ScopeBase::init(mTable);
	}

	void GlobalScopeBase::finalize()
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

	void GlobalScopeBase::executeString(const std::string &cmd) {
		Parsing::ScriptParser::getSingleton().executeString(mState, cmd);
	}

	void GlobalScopeBase::pushScope(int table) {
		lua_rawgeti(mState, LUA_REGISTRYINDEX, mTable);
		lua_pushliteral(mState, "__scopes__");
		lua_rawget(mState, -2);
		lua_replace(mState, -2);
		lua_rawgeti(mState, -1, table);
		lua_replace(mState, -2);
	}

	int GlobalScopeBase::registerScope(ScopeBase *scope, int tableId) {

		lua_rawgeti(mState, LUA_REGISTRYINDEX, mTable);
		lua_pushliteral(mState, "__scopes__");
		lua_rawget(mState, -2);
		lua_replace(mState, -2);

		if (tableId == -1) {		
			lua_newtable(mState);
		}
		else {
			lua_rawgeti(mState, LUA_REGISTRYINDEX, tableId);
		}

		lua_pushlightuserdata(mState, scope);

		lua_setfield(mState, -2, "___scope___");		

		luaL_setmetatable(mState, "Interfaces.Scope");

		int i = luaL_ref(mState, -2);

		lua_pop(mState, 1);

		return i;
	}

	void GlobalScopeBase::unregisterScope(ScopeBase *scope) {
		lua_rawgeti(mState, LUA_REGISTRYINDEX, mTable);
		lua_pushliteral(mState, "__scopes__");
		lua_rawget(mState, -2);
		lua_replace(mState, -2);
		luaL_unref(mState, -1, scope->mTable);
		lua_pop(mState, 1);
	}

	bool GlobalScopeBase::hasMethod(ScopeBase *scope, const std::string &name) {
		lua_rawgeti(mState, LUA_REGISTRYINDEX, mTable);
		lua_pushliteral(mState, "__scopes__");
		lua_rawget(mState, -2);
		lua_replace(mState, -2);
		lua_rawgeti(mState, -1, scope->mTable);

		lua_getfield(mState, -1, name.c_str());

		bool result = lua_isfunction(mState, -1);

		lua_pop(mState, 3);

		return result;
	}

	ArgumentList GlobalScopeBase::callMethod(ScopeBase *scope, const std::string &name, const ArgumentList &args) {
		int level = lua_gettop(mState);

		lua_rawgeti(mState, LUA_REGISTRYINDEX, mTable);
		lua_pushliteral(mState, "__scopes__");
		lua_rawget(mState, -2);
		lua_replace(mState, -2);
		lua_rawgeti(mState, -1, scope->mTable);
		lua_replace(mState, -2);

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


	KeyValueMapList GlobalScopeBase::maps()
	{
		return ScopeBase::maps().merge(mGlobals);
	}

	lua_State * GlobalScopeBase::lua_state()
	{
		return mState;
	}

}
}
