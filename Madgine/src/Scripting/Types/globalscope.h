#pragma once

#include "scopeimpl.h"
#include "scene.h"
#include "uniquecomponentcollector.h"
#include "baseglobalapicomponent.h"

namespace Engine {

#ifdef _MSC_VER
	template MADGINE_EXPORT class UniqueComponentCollector<Scripting::BaseGlobalAPIComponent>;
#endif

namespace Scripting {

class MADGINE_EXPORT GlobalScope : public ScopeImpl<GlobalScope>, public Ogre::Singleton<GlobalScope>, public Util::MadgineObject<GlobalScope> {
public:
    GlobalScope(Parsing::ScriptParser *scriptParser);

	virtual void init() override;
	virtual void finalize() override;

	void addAPI(BaseAPI *api);
	void removeAPI(BaseAPI *api);

	void log(const ValueType &v);
	Struct *createStruct();
	List *createList();
	const ValueType &debug(const ValueType &v);
	Scene *level();
	Struct *getData(const std::string &name);


    
	bool hasScriptMethod(const std::string &name) override;
	virtual void clear() override;

	virtual ValueType methodCall(const std::string &name, const ArgumentList &args = {}) override;

	std::set<BaseGlobalAPIComponent*> getGlobalAPIComponents();

protected:    
    virtual std::string getIdentifier() override;

	virtual const Parsing::MethodNodePtr &getMethod(const std::string &name) override;

private:
    Scene mLevel;

    std::list<BaseAPI*> mAPIs;
	UniqueComponentCollector<BaseGlobalAPIComponent> mGlobalAPIs;

	Parsing::ScriptParser *mScriptParser;

};

}
}

