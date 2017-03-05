#pragma once

#include "Scripting/Types/scopeimpl.h"
#include "scene.h"
#include "uniquecomponentcollector.h"
#include "baseglobalapicomponent.h"
#include "Scripting/Types/refscopetoplevelserializableunit.h"
#include "Scripting\Types\globalscope.h"

namespace Engine {
namespace Scripting {

#ifdef _MSC_VER
	template MADGINE_BASE_EXPORT class UniqueComponentCollector<BaseGlobalAPIComponent>;
#endif

class MADGINE_BASE_EXPORT GlobalScopeImpl : public ScopeImpl<GlobalScopeImpl, GlobalScope>, public Singleton<GlobalScopeImpl> {
public:
    GlobalScopeImpl();

	bool init();
	void finalize();

	void addAPI(BaseAPI *api);
	void removeAPI(BaseAPI *api);

	virtual void clear() override;

	virtual ValueType methodCall(const std::string &name, const ArgumentList &args = {}) override;

	using Singleton<GlobalScopeImpl>::getSingleton;


private:

    std::list<BaseAPI*> mAPIs;
	UniqueComponentCollector<BaseGlobalAPIComponent> mGlobalAPIs;

};

}
}

