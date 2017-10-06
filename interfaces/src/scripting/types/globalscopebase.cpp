#include "interfaceslib.h"
#include "globalscopebase.h"

#include "scripting/types/luastate.h"

#include "generic/keyvalue.h"


namespace Engine {
namespace Scripting {

	GlobalScopeBase::GlobalScopeBase(const LuaTable &table) :
		ScopeBase(table),
		mTable(table)
	{

		assert(mTable);

		mScopes = mTable.createTable();

		mTable.setMetatable("Interfaces.GlobalScope");
		
	}

	void GlobalScopeBase::executeString(const std::string &cmd) {
		LuaState::getSingleton().executeString(mTable.state(), cmd);
	}

	LuaTable GlobalScopeBase::createTable()
	{
		return mScopes.createTable();
	}

	lua_State * GlobalScopeBase::lua_state()
	{
		return mTable.state();
	}

	LuaTable GlobalScopeBase::table()
	{
		return mTable;
	}


}
}
