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

	void addGlobal(ScopeBase *api);
	void removeGlobal(ScopeBase *api);

	virtual KeyValueMapList maps() override;
	
	static const int sNoRef;

	lua_State *lua_state();

	LuaTable createTable();

private:
	LuaTable mTable;
	LuaTable mScopes;
	std::map<std::string, ScopeBase*> mGlobals;
};

}
}



