#include "interfaceslib.h"
#include "globalscopebase.h"

#include "scripting/parsing/scriptparser.h"

#include "scopebase.h"


namespace Engine {
namespace Scripting {

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

		return true;
	}

	void GlobalScopeBase::finalize()
	{
		ScopeBase::finalize();
		mTable.clear();
		mScopes.clear();
	}

	void GlobalScopeBase::executeString(const std::string &cmd) {
		Parsing::ScriptParser::getSingleton().executeString(mTable.state(), cmd);
	}

	LuaTable GlobalScopeBase::createTable()
	{
		return mScopes.createTable();
	}


	LuaTable GlobalScopeBase::table()
	{
		return mTable;
	}

	lua_State * GlobalScopeBase::lua_state()
	{
		return mTable.state();
	}

}
}
