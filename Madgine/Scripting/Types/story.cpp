#include "libinclude.h"
#include "story.h"
#include "Scripting/Parsing/scriptparser.h"
#include "struct.h"
#include "list.h"
#include "globalapi.h"
#include "Database\exceptionmessages.h"
#include "OGRE\scenemanager.h"
#include "OGRE\Entity\entity.h"

namespace Engine {
namespace Scripting {

//Story::Factory Story::sFactory;

	API_IMPL(Story, &log, "Struct"_(&createStruct), "List"_(&createList), &debug, &level, &getData);


Story::Story(Parsing::ScriptParser *scriptParser) :
	mScriptParser(scriptParser)
{    
	Ogre::LogManager::getSingleton().createLog("Scripting.log");
}

void Story::init() {
	mGlobalAPIs = OGRE_MAKE_UNIQUE(UniqueComponentCollector<BaseGlobalAPIComponent>)();
	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &api : *mGlobalAPIs) {
		api->init();
	}
}

Level *Story::level()
{
    return &mLevel;
}

void Story::addAPI(BaseAPI * api)
{
	mAPIs.push_back(api);
}

void Story::removeAPI(BaseAPI * api)
{
	mAPIs.remove(api);
}


bool Story::hasScriptMethod(const std::string &name)
{
	return mScriptParser->hasGlobalMethod(name);
}

Struct *Story::getData(const std::string &name)
{
	return &mScriptParser->getPrototype(name);
}

const Parsing::MethodNodePtr &Story::getMethod(const std::string &name)
{
	return mScriptParser->getGlobalMethod(name);
}

void Story::log(const ValueType &v)
{
	Util::UtilMethods::log(v.toString(), Ogre::LML_NORMAL);
}

Struct *Story::createStruct()
{
    return OGRE_NEW Struct();
}

List *Story::createList()
{
    return OGRE_NEW List();
}

const ValueType &Story::debug(const ValueType &v)
{
    return v;
}



std::string Story::getIdentifier()
{
    return "Global Scope";
}

void Story::clear()
{
    Scope::clear();
    mLevel.clear();
}


ValueType Story::methodCall(const std::string &name, const Scripting::ArgumentList &args)
{
	for (BaseAPI *api : mAPIs) {
		if (api->hasMethod(name)) {
			TRACE_S("<unknown>", -1, name);
			return api->execMethod(name, args);
		}
	}
 
    return ScopeImpl::methodCall(name, args);
}

Scope *Story::Factory::create(Serialize::SerializeInStream &in)
{
    return &Story::getSingleton();
}

}
}
