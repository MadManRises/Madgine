#include "../../interfaceslib.h"
#include "globalscopebase.h"

#include "../types/luastate.h"


namespace Engine
{

	SINGLETON_IMPL(Scripting::GlobalScopeBase);

	namespace Scripting
	{


		GlobalScopeBase::GlobalScopeBase(const LuaTable& table) :
			ScopeBase(table),
			mTable(table)
		{
			assert(mTable);

			mScopes = mTable.createTable();

			mTable.setMetatable("Interfaces.GlobalScope");
		}

		void GlobalScopeBase::executeString(const std::string& cmd)
		{
			LuaState::getSingleton().executeString(mTable.state(), cmd);
		}

		LuaTable GlobalScopeBase::createTable()
		{
			return mScopes.createTable();
		}

		lua_State* GlobalScopeBase::lua_state() const
		{
			return mTable.state();
		}

		LuaTable GlobalScopeBase::table() const
		{
			return mTable;
		}
	}
}
