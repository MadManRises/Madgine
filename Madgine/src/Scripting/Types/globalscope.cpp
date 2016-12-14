#include "madginelib.h"
#include "globalscope.h"
#include "Scripting/Parsing/scriptparser.h"
#include "struct.h"
#include "list.h"
#include "array.h"
#include "globalapi.h"
#include "Database\exceptionmessages.h"

namespace Engine {
namespace Scripting {

	API_IMPL(GlobalScope);


	GlobalScope::GlobalScope(Serialize::TopLevelSerializableUnit *parent, Parsing::ScriptParser *scriptParser) :
		mScriptParser(scriptParser),
		ScopeImpl(parent)
	{
		Ogre::LogManager::getSingleton().createLog("Scripting.log");
	}

void GlobalScope::init() {

	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		api->init();
	}
}

void GlobalScope::finalize()
{
	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		api->finalize();
	}
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

const Parsing::MethodNodePtr &GlobalScope::getMethod(const std::string &name)
{
	return mScriptParser->getGlobalMethod(name);
}


void GlobalScope::clear()
{
	for (const Ogre::unique_ptr<BaseGlobalAPIComponent> &p : mGlobalAPIs) {
		p->clear();
	}
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


}



}
