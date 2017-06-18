#pragma once

#include "serialize/container/observed.h"
#include "serialize/container/map.h"
#include "scripting/datatypes/argumentlist.h"
#include "scopebase.h"

namespace Engine {
namespace Scripting {


class INTERFACES_EXPORT GlobalScopeBase : public Singleton<GlobalScopeBase>, public ScopeBase {

public:
	GlobalScopeBase(lua_State *state);
	
	virtual bool init() override;
	virtual void finalize() override;

	void pushScope(int table);

	int registerScope(ScopeBase *scope);
	void unregisterScope(int table);

	bool hasMethod(ScopeBase *scope, const std::string &name);
	ArgumentList callMethod(ScopeBase *scope, const std::string &name, const ArgumentList &args = {});


	void addGlobal(ScopeBase *api);
	void removeGlobal(ScopeBase *api);

	virtual KeyValueMapList maps() override;
	
	static const int sNoRef;


private:


	lua_State *mState;
	std::map<std::string, ScopeBase*> mGlobals;
};

}
}



