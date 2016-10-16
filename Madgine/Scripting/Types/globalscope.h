#pragma once

#include "scopeimpl.h"
#include "scopefactoryimpl.h"
#include "level.h"
#include "uniquecomponentcollector.h"
#include "baseglobalapicomponent.h"

namespace Engine {
namespace Scripting {

	template MADGINE_EXPORT class UniqueComponentCollector<BaseGlobalAPIComponent>;

class MADGINE_EXPORT GlobalScope : public ScopeImpl<GlobalScope>, public Ogre::Singleton<GlobalScope> {
public:
    GlobalScope(Parsing::ScriptParser *scriptParser);

	void init();

	void addAPI(BaseAPI *api);
	void removeAPI(BaseAPI *api);

	void log(const ValueType &v);
	Struct *createStruct();
	List *createList();
	const ValueType &debug(const ValueType &v);
	Level *level();
	Struct *getData(const std::string &name);


    
	bool hasScriptMethod(const std::string &name) override;
	virtual void clear() override;

	virtual ValueType methodCall(const std::string &name, const ArgumentList &args = {}) override;

protected:    
    virtual std::string getIdentifier() override;

	virtual const Parsing::MethodNodePtr &getMethod(const std::string &name) override;

private:
    Level mLevel;

    std::list<BaseAPI*> mAPIs;
	Ogre::unique_ptr<UniqueComponentCollector<BaseGlobalAPIComponent>> mGlobalAPIs;

	Parsing::ScriptParser *mScriptParser;

};

}
}

