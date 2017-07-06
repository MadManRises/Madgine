#pragma once

#include "serialize/container/observed.h"
#include "serialize/container/map.h"
#include "scripting/datatypes/argumentlist.h"
#include "scopebase.h"

namespace Engine {
namespace Scripting {


class INTERFACES_EXPORT GlobalScopeBase : public Singleton<GlobalScopeBase>, public ScopeBase {

public:
	GlobalScopeBase(const std::pair<lua_State *, int> &state);
	
	virtual bool init() override;
	virtual void finalize() override;

	void pushScope(int table);

	int registerScope(ScopeBase *scope, int tableId);
	void unregisterScope(ScopeBase *scope);

	bool hasMethod(ScopeBase *scope, const std::string &name);
	ArgumentList callMethod(ScopeBase *scope, const std::string &name, const ArgumentList &args = {});

	void executeString(const std::string &cmd);

	void addGlobal(ScopeBase *api);
	void removeGlobal(ScopeBase *api);

	virtual KeyValueMapList maps() override;
	
	static const int sNoRef;

	lua_State *lua_state();

private:


	lua_State *mState;
	int mTable;
	std::map<std::string, ScopeBase*> mGlobals;
};

}
}



