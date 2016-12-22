#pragma once

#include "scopeimpl.h"
#include "scene.h"
#include "uniquecomponentcollector.h"
#include "baseglobalapicomponent.h"
#include "refscopetoplevelserializableunit.h"

namespace Engine {

#ifdef _MSC_VER
	template MADGINE_EXPORT class UniqueComponentCollector<Scripting::BaseGlobalAPIComponent>;
#endif

namespace Scripting {

class MADGINE_EXPORT GlobalScope : public ScopeImpl<GlobalScope>, public Ogre::Singleton<GlobalScope> {
public:
    GlobalScope(Parsing::ScriptParser *scriptParser);

	void init();
	void finalize();

	void addAPI(BaseAPI *api);
	void removeAPI(BaseAPI *api);

    
	bool hasScriptMethod(const std::string &name) override;
	virtual void clear() override;

	virtual ValueType methodCall(const std::string &name, const ArgumentList &args = {}) override;

	std::set<BaseGlobalAPIComponent*> getGlobalAPIComponents();

protected:    

	virtual const Parsing::MethodNodePtr &getMethod(const std::string &name) override;

private:

    std::list<BaseAPI*> mAPIs;
	UniqueComponentCollector<BaseGlobalAPIComponent> mGlobalAPIs;

	Parsing::ScriptParser *mScriptParser;

};

}
}

