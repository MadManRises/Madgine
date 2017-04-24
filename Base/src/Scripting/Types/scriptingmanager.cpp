#include "baselib.h"
#include "scriptingmanager.h"
#include "Scripting/Parsing/scriptparser.h"
#include "globalapi.h"

#include "Serialize/TopLevelIds.h"

namespace Engine {
namespace Scripting {

	API_IMPL(ScriptingManager, &log, &createGlobalStruct, &createGlobalList, &createGlobalArray, &debug, &scene, &getPrototype);


	ScriptingManager::ScriptingManager() :
		RefScopeTopLevelSerializableUnit(Serialize::SCRIPTING_MANAGER),
		mScriptParser(&Parsing::ScriptParser::getSingleton())
	{
		//Ogre::LogManager::getSingleton().createLog("Scripting.log");
		mGlobalScope.addAPI(this);
	}

bool ScriptingManager::init() {
	return mGlobalScope.init() && MadgineObject::init();
}

void ScriptingManager::finalize()
{
	mGlobalScope.finalize();
	MadgineObject::finalize();
}

Scene *ScriptingManager::scene()
{
    return &mScene;
}


Prototype *ScriptingManager::getPrototype(const std::string &name)
{
	return &mScriptParser->getPrototype(name);
}


void ScriptingManager::log(const ValueType &v)
{
	Util::UtilMethods::log(v.toString(), Util::LOG_TYPE);
}

Struct *ScriptingManager::createGlobalStruct()
{
    return createStruct();
}


List *ScriptingManager::createGlobalList()
{
    return createList();
}

Vector * ScriptingManager::createGlobalArray(size_t size)
{
	Scripting::Vector *v = createVector();
	v->resize(size);
	return v;
}

const ValueType &ScriptingManager::debug(const ValueType &v)
{
    return v;
}


void ScriptingManager::clear()
{
	
	mScene.clear();
    
}

GlobalScope * ScriptingManager::globalScope()
{
	return &mGlobalScope;
}



}



}
