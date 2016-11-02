#include "libinclude.h"
#include "globalscope.h"
#include "Scripting/Parsing/scriptparser.h"
#include "struct.h"
#include "list.h"
#include "globalapi.h"
#include "Database\exceptionmessages.h"

namespace Engine {
namespace Scripting {

//Story::Factory Story::sFactory;

	API_IMPL(GlobalScope, &log, &createStruct, &createList, &debug, &level, &getData);


	GlobalScope::GlobalScope(Parsing::ScriptParser *scriptParser) :
	mScriptParser(scriptParser)
{    
	Ogre::LogManager::getSingleton().createLog("Scripting.log");
}

void GlobalScope::init() {
	MadgineObject::init();

	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		api->init();
	}
}

void GlobalScope::finalize()
{
	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		api->finalize();
	}
	MadgineObject::finalize();
}

Level *GlobalScope::level()
{
    return &mLevel;
}

void GlobalScope::addAPI(BaseAPI * api)
{
	mAPIs.push_back(api);
}

void GlobalScope::removeAPI(BaseAPI * api)
{
	mAPIs.remove(api);
}


bool GlobalScope::hasScriptMethod(const std::string &name)
{
	return mScriptParser->hasGlobalMethod(name);
}

Struct *GlobalScope::getData(const std::string &name)
{
	return &mScriptParser->getPrototype(name);
}

const Parsing::MethodNodePtr &GlobalScope::getMethod(const std::string &name)
{
	return mScriptParser->getGlobalMethod(name);
}

void GlobalScope::log(const ValueType &v)
{
	Util::UtilMethods::log(v.toString(), Ogre::LML_NORMAL);
}

Struct *GlobalScope::createStruct()
{
    return OGRE_NEW Struct();
}

List *GlobalScope::createList()
{
    return OGRE_NEW List();
}

const ValueType &GlobalScope::debug(const ValueType &v)
{
    return v;
}



std::string GlobalScope::getIdentifier()
{
    return "Global Scope";
}

void GlobalScope::clear()
{
	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &p : mGlobalAPIs) {
		p->clear();
	}
	mLevel.clear();
    Scope::clear();    
}


ValueType GlobalScope::methodCall(const std::string &name, const Scripting::ArgumentList &args)
{
	for (BaseAPI *api : mAPIs) {
		if (api->hasMethod(name)) {
			TRACE_S("<unknown>", -1, name);
			return api->execMethod(name, args);
		}
	}
 
    return ScopeImpl::methodCall(name, args);
}

std::set<BaseGlobalAPIComponent*> GlobalScope::getGlobalAPIComponents()
{
	std::set<BaseGlobalAPIComponent*> result;
	for (const std::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		result.insert(api.get());
	}
	return result;
}

template <> Scope *GlobalScope::Factory::create(Serialize::SerializeInStream &in)
{
    return &GlobalScope::getSingleton();
}

}
}
