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

	GlobalScopeBase::GlobalScopeBase(const LuaTable &table) :
	mTable(table)
	{
		

	}

	bool GlobalScopeBase::init()
	{
		if (!mTable) {
			mTable = Engine::Scripting::Parsing::ScriptParser::getSingleton().createThread();
		}

		mScopes = mTable.createTable();

		if (!ScopeBase::init(mTable))
			return false;

		mTable.setMetatable("Interfaces.GlobalScope");

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
		Parsing::ScriptParser::getSingleton().executeString(mTable.state(), cmd);
	}

	LuaTable GlobalScopeBase::createTable()
	{
		return mScopes.createTable();
	}


	


	KeyValueMapList GlobalScopeBase::maps()
	{
		return ScopeBase::maps().merge(mGlobals);
	}

	lua_State * GlobalScopeBase::lua_state()
	{
		return mTable.state();
	}

}
}
