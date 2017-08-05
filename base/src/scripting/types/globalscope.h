#pragma once


#include "uniquecomponentcollector.h"
#include "globalapicomponentbase.h"
#include "scripting/types/globalscopebase.h"


namespace Engine {

#ifdef _MSC_VER
	template class MADGINE_BASE_EXPORT BaseUniqueComponentCollector<Scripting::GlobalAPIComponentBase>;
#endif

	namespace Scripting {

class MADGINE_BASE_EXPORT GlobalScope : public GlobalScopeBase, public Singleton<GlobalScope> {
public:
    GlobalScope(const LuaTable &, App::Application *app);

	virtual bool init() override;
	virtual void finalize() override;

	virtual KeyValueMapList maps() override;


	void clear();

	void update();

	using Singleton<GlobalScope>::getSingleton;


private:
	BaseUniqueComponentCollector<GlobalAPIComponentBase> mGlobalAPIs;

};

}
}

