#pragma once

#include "serialize/container/observed.h"
#include "serialize/container/map.h"
#include "scripting/datatypes/argumentlist.h"
#include "scopebase.h"

namespace Engine {
namespace Scripting {


class INTERFACES_EXPORT GlobalScopeBase : public Singleton<GlobalScopeBase>, public ScopeBase {

public:
	GlobalScopeBase(const LuaTable &table);
	
	virtual bool init() override;
	virtual void finalize() override;

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



