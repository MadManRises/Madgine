#include "interfaceslib.h"
#include "scopebase.h"

#include "globalscopebase.h"

#include "serialize/toplevelserializableunit.h"

#include "api.h"


namespace Engine {
namespace Scripting {


ScopeBase::ScopeBase() :
	mGlobal(nullptr),
	mTable(GlobalScopeBase::sNoRef)
{
	
}

ScopeBase::~ScopeBase()
{
}

bool Engine::Scripting::ScopeBase::init()
{
	if (!mGlobal) {
		mGlobal = GlobalScopeBase::getSingletonPtr();
		if (!mGlobal)
			return false;
	}
	if (mTable != GlobalScopeBase::sNoRef)
		return false;
	mTable = mGlobal->registerScope(this);
	return true;
}

void ScopeBase::finalize() {
	if (mTable != GlobalScopeBase::sNoRef) {
		mGlobal->unregisterScope(mTable);
		mTable = GlobalScopeBase::sNoRef;
	}
}

bool ScopeBase::initGlobal(int tableId)
{
	if (!mGlobal) {
		mGlobal = GlobalScopeBase::getSingletonPtr();
		if (!mGlobal)
			return false;
	}
	if (mTable != GlobalScopeBase::sNoRef)
		return false;
	mTable = tableId;
	return true;
}

void Engine::Scripting::ScopeBase::push()
{
	mGlobal->pushScope(mTable);
}

ArgumentList ScopeBase::methodCall(const std::string &name, const ArgumentList &args)
{
	return mGlobal->callMethod(this, name, args);
}

std::pair<bool, ArgumentList> ScopeBase::callMethodIfAvailable(const std::string & name, const ArgumentList & args)
{
	if (hasMethod(name))
		return callMethodCatch(name, args);
	else
		return std::make_pair(false, ArgumentList());
}

std::pair<bool, ArgumentList> ScopeBase::callMethodCatch(const std::string & name, const ArgumentList & args)
{
	try {
		return std::make_pair(true, methodCall(name, args));
	}
	catch (std::exception &e) {
		LOG_EXCEPTION(e);
	}
	return std::make_pair(false, ArgumentList());
}


bool ScopeBase::hasMethod(const std::string &name)
{
	return mGlobal->hasMethod(this, name);
}

std::string ScopeBase::getIdentifier()
{
	return typeid(*this).name();
}

std::string ScopeBase::getName() {
	throw 0;
}

int ScopeBase::table() {
	return mTable;
}

int ScopeBase::resolve(lua_State *state, const std::string &key) {
	return maps().resolve(state, key);
}

KeyValueIterator *Engine::Scripting::ScopeBase::iterator()
{
	return maps().iterator();
}

KeyValueMapList Engine::Scripting::ScopeBase::maps()
{
	return KeyValueMapList();
}

GlobalScopeBase * ScopeBase::globalScope()
{
	return mGlobal;
}

}
}
