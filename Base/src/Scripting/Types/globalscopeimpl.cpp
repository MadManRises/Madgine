#include "baselib.h"
#include "globalscopeimpl.h"
#include "Scripting/Parsing/scriptparser.h"
#include "globalapi.h"
#include "exceptionmessages.h"

namespace Engine {
namespace Scripting {


	API_IMPL(GlobalScopeImpl);


	GlobalScopeImpl::GlobalScopeImpl(Parsing::ScriptParser *scriptParser) :
		mScriptParser(scriptParser)
	{
	}

void GlobalScopeImpl::init() {

	for (const std::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		api->init();
	}
}

void GlobalScopeImpl::finalize()
{
	for (const std::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		api->finalize();
	}
}

void GlobalScopeImpl::addAPI(BaseAPI * api)
{
	mAPIs.push_back(api);
}

void GlobalScopeImpl::removeAPI(BaseAPI * api)
{
	mAPIs.remove(api);
}


bool GlobalScopeImpl::hasScriptMethod(const std::string &name)
{
	return mScriptParser->hasGlobalMethod(name);
}

const Parsing::MethodNode &GlobalScopeImpl::getMethod(const std::string &name)
{
	return mScriptParser->getGlobalMethod(name);
}


void GlobalScopeImpl::clear()
{
	for (const std::unique_ptr<BaseGlobalAPIComponent> &p : mGlobalAPIs) {
		p->clear();
	}
    Scope::clear();    
}


ValueType GlobalScopeImpl::methodCall(const std::string &name, const Scripting::ArgumentList &args)
{
	for (BaseAPI *api : mAPIs) {
		if (api->hasMethod(name)) {
			TRACE_S("<unknown>", -1, name);
			return api->execMethod(name, args);
		}
	}
 
    return ScopeImpl::methodCall(name, args);
}

std::set<BaseGlobalAPIComponent*> GlobalScopeImpl::getGlobalAPIComponents()
{
	std::set<BaseGlobalAPIComponent*> result;
	for (const std::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		result.insert(api.get());
	}
	return result;
}


}



}