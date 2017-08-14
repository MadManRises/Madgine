#include "baselib.h"
#include "globalscope.h"

#include "app/application.h"

namespace Engine {
namespace Scripting {



	GlobalScope::GlobalScope(const LuaTable &table, App::Application *app) :
		GlobalScopeBase(table, app)
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

KeyValueMapList GlobalScope::maps()
{
	return GlobalScopeBase::maps().merge(mGlobalAPIs);
}


const char *GlobalScope::key() const
{
	throw 0;
}

}



}
