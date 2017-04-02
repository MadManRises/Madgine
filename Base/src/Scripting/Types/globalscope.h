#pragma once

#include "Scripting/Types/scope.h"
#include "scene.h"
#include "uniquecomponentcollector.h"
#include "GlobalAPIComponentBase.h"
#include "Scripting/Types/refscopetoplevelserializableunit.h"
#include "Scripting\Types\globalscopebase.h"

namespace Engine {
namespace Scripting {

#ifdef _MSC_VER
	template MADGINE_BASE_EXPORT class UniqueComponentCollector<GlobalAPIComponentBase, BaseCreatorStore<GlobalAPIComponentBase>>;
#endif

class MADGINE_BASE_EXPORT GlobalScope : public Scope<GlobalScope, GlobalScopeBase>, public Singleton<GlobalScope> {
public:
    GlobalScope();

	bool init();
	void finalize();

	void addAPI(APIBase *api);
	void removeAPI(APIBase *api);

	virtual void clear() override;

	virtual ValueType methodCall(const std::string &name, const ArgumentList &args = {}) override;

	using Singleton<GlobalScope>::getSingleton;

	void update(float timeSinceLastFrame);

private:

    std::list<APIBase*> mAPIs;
	BaseUniqueComponentCollector<GlobalAPIComponentBase> mGlobalAPIs;

};

}
}

