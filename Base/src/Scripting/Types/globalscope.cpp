#include "baselib.h"
#include "GlobalScope.h"
#include "Scripting/Parsing/scriptparser.h"


namespace Engine {
namespace Scripting {



	GlobalScope::GlobalScope(lua_State *state) :
		GlobalScopeBase(state)
	{
	}

bool GlobalScope::init() {
	if (!GlobalScopeBase::init())
		return false;
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
	GlobalScopeBase::finalize();
}


void GlobalScope::clear()
{
	for (const std::unique_ptr<GlobalAPIComponentBase> &p : mGlobalAPIs) {
		p->clear();
	}   
}


/*void GlobalScope::log(const ValueType &v)
{
	Util::UtilMethods::log(v.toString(), Util::LOG_TYPE);
}*/


void GlobalScope::update()
{
	for (const std::unique_ptr<GlobalAPIComponentBase> &p : mGlobalAPIs) {
		p->update();
	}
}


}



}
