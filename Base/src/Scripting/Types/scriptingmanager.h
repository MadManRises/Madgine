#pragma once

#include "scene.h"
#include "globalscopeimpl.h"
#include "Scripting\Types\refscopetoplevelserializableunit.h"

namespace Engine {
namespace Scripting {

class MADGINE_BASE_EXPORT ScriptingManager : public RefScopeTopLevelSerializableUnit, public Singleton<ScriptingManager>, public MadgineObject<ScriptingManager>, public API<ScriptingManager> {
public:
    ScriptingManager();

	virtual bool init() override;
	virtual void finalize() override;

	void log(const ValueType &v);
	Struct *createGlobalStruct();
	List *createGlobalList();
	Vector *createGlobalArray(size_t size);
	const ValueType &debug(const ValueType &v);
	Scene *scene();
	Prototype *getPrototype(const std::string &name);

	void clear();

	GlobalScopeImpl *globalScope();

private:
    Scene mScene;

	GlobalScopeImpl mGlobalScope;

	Parsing::ScriptParser *mScriptParser;

};

}
}

