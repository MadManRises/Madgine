#include "libinclude.h"
#include "levelserializer.h"

#include "Scripting/Types/globalscope.h"
#include "Ogre/SceneManager.h"
#include "Ogre/Entity/entity.h"

#include "Scripting/Types/list.h"
#include "Scripting/Types/struct.h"

#include "Scripting/Types/scopefactory.h"
#include "serializestream.h"

#include "UI/Process.h"

namespace Engine {
namespace Scripting {
namespace Serialize {

void LevelSerializer::storeCurrentLevel(SerializeOutStream &out, bool storeComponents, bool saveStory)
{

	GlobalScope *global = &GlobalScope::getSingleton();

    out << OGRE::SceneManager::getSingleton();

    if (storeComponents)
        OGRE::SceneManager::getSingleton().saveComponentData(out);
    else
        out << Scripting::ValueType();

    std::set<Scope *> scopeSet, ignoreSet;

    global->collectScopes(saveStory ? scopeSet : ignoreSet);
	
	for (OGRE::Entity::Entity *e : OGRE::SceneManager::getSingleton().entities()) {
		e->collectScopes(scopeSet, ignoreSet);
	}

    global->level()->collectScopes(scopeSet, ignoreSet);

    for (Scope *s : scopeSet){
        out << s;
        s->storeCreationData(out);
    }

    out << ValueType();


    for (Scope *s : scopeSet){
        out << s << *s;
    }

    out << ValueType();
}

void LevelSerializer::restoreLevel(SerializeInStream &in, bool callInit)
{

	if (in.process())
		in.process()->startSubProcess(1, "Loading Level...");

    in >> OGRE::SceneManager::getSingleton();

    OGRE::SceneManager::getSingleton().loadComponentData(in);

    std::map<InvScopePtr, Scope *> scopeMap;

    ValueType scopeIn;

	ScopeFactoryManager *factory = &ScopeFactoryManager::getSingleton();
	InvScopePtr oldPtr;
    while(in.loopRead(oldPtr)){
        scopeMap[oldPtr] = factory->create(in);
    }

    while(in.loopRead(oldPtr)){
        auto it = scopeMap.find(oldPtr);
        if (it == scopeMap.end()) throw 0;
        Scope *scope = it->second;
        in >> *scope;
        scope->applyScopeMap(scopeMap);

		if (callInit) {
			OGRE::Entity::Entity *e = scope_cast<OGRE::Entity::Entity>(scope);
			if (e) {
				e->init();
			}
		}
    }

    OGRE::SceneManager::getSingleton().onLoad();

	if (in.process())
		in.process()->endSubProcess();

}

}
} // namespace Scripting
} // namespace Core

