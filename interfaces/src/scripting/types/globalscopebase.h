#pragma once

#include "scopebase.h"

namespace Engine {
namespace Scripting {


class INTERFACES_EXPORT GlobalScopeBase : public Singleton<GlobalScopeBase>, public ScopeBase {

public:
	GlobalScopeBase(const LuaTable &table, ScopeBase *topLevelApi = nullptr);
	
	virtual bool init() override;
	virtual void finalize() override;

	void executeString(const std::string &cmd);

	lua_State *lua_state();

	LuaTable createTable();
	LuaTable table();

	virtual KeyValueMapList maps() override;

private:
	LuaTable mTable;
	LuaTable mScopes;

	ScopeBase *mTopLevelApi;
};

}
}



