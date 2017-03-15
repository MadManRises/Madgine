#include "baselib.h"
#include "GlobalScope.h"
#include "Scripting/Parsing/scriptparser.h"
#include "globalapi.h"
#include "exceptionmessages.h"

namespace Engine {
namespace Scripting {


	API_IMPL(GlobalScope);


	GlobalScope::GlobalScope() 
	{
	}

bool GlobalScope::init() {

	for (const std::unique_ptr<GlobalAPIComponentBase> &api : mGlobalAPIs) {
		if (!api->init())
			return false;
	}
	return true;
}

void GlobalScope::finalize()
{
	for (const std::unique_ptr<GlobalAPIComponentBase> &api : mGlobalAPIs) {
		api->finalize();
	}
}

void GlobalScope::addAPI(APIBase * api)
{
	mAPIs.push_back(api);
}

void GlobalScope::removeAPI(APIBase * api)
{
	mAPIs.remove(api);
}

void GlobalScope::clear()
{
	for (const std::unique_ptr<GlobalAPIComponentBase> &p : mGlobalAPIs) {
		p->clear();
	}
    Scope::clear();    
}


ValueType GlobalScope::methodCall(const std::string &name, const Scripting::ArgumentList &args)
{
	for (APIBase *api : mAPIs) {
		if (api->hasMethod(name)) {
			TRACE_S("<unknown>", -1, name);
			return api->execMethod(name, args);
		}
	}
 
    return Scope::methodCall(name, args);
}

void GlobalScope::update(float timeSinceLastFrame)
{
	for (const std::unique_ptr<GlobalAPIComponentBase> &p : mGlobalAPIs) {
		p->update(timeSinceLastFrame);
	}
}


}



}
