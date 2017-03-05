#include "baselib.h"
#include "globalscopeimpl.h"
#include "Scripting/Parsing/scriptparser.h"
#include "globalapi.h"
#include "exceptionmessages.h"

namespace Engine {
namespace Scripting {


	API_IMPL(GlobalScopeImpl);


	GlobalScopeImpl::GlobalScopeImpl() 
	{
	}

bool GlobalScopeImpl::init() {

	for (const std::unique_ptr<BaseGlobalAPIComponent> &api : mGlobalAPIs) {
		if (!api->init())
			return false;
	}
	return true;
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


}



}
