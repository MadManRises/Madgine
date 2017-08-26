#pragma once

#include "scopebase.h"

namespace Engine {
namespace Scripting {


class INTERFACES_EXPORT GlobalScopeBase : public Singleton<GlobalScopeBase>, public ScopeBase {

public:
	GlobalScopeBase(const LuaTable &table);

	void executeString(const std::string &cmd);

	lua_State *lua_state();

	LuaTable createTable();
	LuaTable table();

private:
	LuaTable mTable;
	LuaTable mScopes;

};

}
}



