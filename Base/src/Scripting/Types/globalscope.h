#pragma once


#include "uniquecomponentcollector.h"
#include "GlobalAPIComponentBase.h"
#include "Scripting\Types\globalscopebase.h"


namespace Engine {

#ifdef _MSC_VER
	template class MADGINE_BASE_EXPORT BaseUniqueComponentCollector<Scripting::GlobalAPIComponentBase>;
#endif

	namespace Scripting {

class MADGINE_BASE_EXPORT GlobalScope : public GlobalScopeBase, public Singleton<GlobalScope> {
public:
    GlobalScope(lua_State *state);

	virtual bool init() override;
	virtual void finalize() override;



	void clear();

	void update();

	using Singleton<GlobalScope>::getSingleton;


private:


	BaseUniqueComponentCollector<GlobalAPIComponentBase> mGlobalAPIs;

};

}
}

