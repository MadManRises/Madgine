#include "interfaceslib.h"
#include "scopebase.h"

#include "globalscopebase.h"

#include "serialize/toplevelserializableunit.h"

#include "api.h"

#include "keyvalue.h"

namespace Engine {
namespace Scripting {


ScopeBase::ScopeBase() :
	mGlobal(nullptr)
{
	
}

ScopeBase::~ScopeBase()
{
}

bool ScopeBase::init()
{
	return init({});
}

void ScopeBase::finalize() {
	mTable.clear();
}

bool ScopeBase::init(const LuaTable &table)
{
	if (!mGlobal) {
		mGlobal = GlobalScopeBase::getSingletonPtr();
		if (!mGlobal)
			return false;
	}
	if (mTable)
		return false;
	mTable = table ? table : mGlobal->createTable();
	mTable.setLightUserdata("___scope___", this);
	mTable.setMetatable("Interfaces.Scope");

	return true;
}

void ScopeBase::push()
{
	mTable.push();
}

ArgumentList ScopeBase::methodCall(const std::string &name, const ArgumentList &args)
{
	return mTable.callMethod(name, args);
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
	return mTable.hasFunction(name);
}

std::string ScopeBase::getIdentifier() const
{
	return typeid(*this).name();
}

std::string ScopeBase::getName() const {
	throw 0;
}


std::pair<bool, ValueType> ScopeBase::get(const std::string &key) {
	return maps().get(key);
}

std::unique_ptr<KeyValueIterator> ScopeBase::iterator()
{
	return maps().iterator();
}

KeyValueMapList ScopeBase::maps()
{
	return KeyValueMapList(this);
}

GlobalScopeBase * ScopeBase::globalScope()
{
	return mGlobal;
}

}
}
