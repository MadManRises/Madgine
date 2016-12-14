#pragma once

#include "scene.h"
#include "globalscope.h"
#include "refscopetoplevelserializableunit.h"

namespace Engine {
namespace Scripting {

class MADGINE_EXPORT ScriptingManager : public RefScopeTopLevelSerializableUnit, public Ogre::Singleton<ScriptingManager>, public Util::MadgineObject<ScriptingManager>, public API<ScriptingManager>, public Ogre::GeneralAllocatedObject {
public:
    ScriptingManager(Parsing::ScriptParser *scriptParser);

	virtual void init() override;
	virtual void finalize() override;

	void log(const ValueType &v);
	Struct *createGlobalStruct();
	Struct *createGlobalStruct_(const std::string &prototype);
	List *createGlobalList();
	Vector *createGlobalArray(size_t size);
	const ValueType &debug(const ValueType &v);
	Scene *scene();
	Prototype *getPrototype(const std::string &name);

	void clear();

	GlobalScope *globalScope();

private:
    Scene mScene;

	GlobalScope mGlobalScope;

	Parsing::ScriptParser *mScriptParser;

};

}
}

