#pragma once


#include "scripting/datatypes/luatable.h"
#include "scripting/datatypes/argumentlist.h"

namespace Engine {
namespace Scripting {

	

class INTERFACES_EXPORT ScopeBase {

public:
	ScopeBase();
	ScopeBase(const ScopeBase &) = delete;
	ScopeBase(ScopeBase &&) = delete;
    virtual ~ScopeBase();

	bool hasMethod(const std::string &name);
 
	ArgumentList methodCall(const std::string &name,
		const ArgumentList &args = {});

	std::pair<bool, ArgumentList> callMethodIfAvailable(const std::string &name, const ArgumentList &args = {});
	std::pair<bool, ArgumentList> callMethodCatch(const std::string &name, const ArgumentList &args = {});

	void push();

	std::pair<bool, ValueType> get(const std::string &key);
	std::unique_ptr<KeyValueIterator> iterator();

	virtual KeyValueMapList maps();

	virtual std::string getIdentifier() const;
	virtual std::string getName() const;

	virtual bool init();
	virtual void finalize();

protected:
	bool init(const LuaTable &table);

	friend class GlobalScopeBase;
	GlobalScopeBase *globalScope();

private:

	GlobalScopeBase *mGlobal;
	LuaTable mTable;
		 
};

}
}


