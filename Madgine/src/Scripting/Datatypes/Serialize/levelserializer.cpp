#include "libinclude.h"
#include "levelserializer.h"

#include "Scripting/Types/globalscope.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity/entity.h"

#include "Scripting/Types/scopefactory.h"
#include "serializestream.h"

#include "UI/Process.h"

namespace Engine {
namespace Scripting {
namespace Serialize {

void LevelSerializer::storeCurrentLevel(SerializeOutStream &out, bool storeComponents, const std::set<Scope*> &ignoreSet)
{

	GlobalScope *global = &GlobalScope::getSingleton();

    out << Scene::SceneManager::getSingleton();

    if (storeComponents)
        Scene::SceneManager::getSingleton().saveComponentData(out);
    else
        out << Scripting::ValueType();


    std::set<Scope *> scopeSet;

    global->collectScopes(scopeSet, ignoreSet);
	
	for (Scene::Entity::Entity *e : Scene::SceneManager::getSingleton().entities()) {
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

    in >> Scene::SceneManager::getSingleton();

    Scene::SceneManager::getSingleton().loadComponentData(in);

    std::map<InvScopePtr, Scope *> scopeMap;

    ValueType scopeIn;

	ScopeFactoryManager *factory = &ScopeFactoryManager::getSingleton();
	InvScopePtr oldPtr;
    while(in.loopRead(oldPtr)){
		Scope *scope = factory->create(in);
		if (!scope) return;
        scopeMap[oldPtr] = scope;

    }

    while(in.loopRead(oldPtr)){
        auto it = scopeMap.find(oldPtr);
        if (it == scopeMap.end()) throw 0;
        Scope *scope = it->second;
        in >> *scope;
        scope->applyScopeMap(scopeMap);

		if (callInit) {
			Scene::Entity::Entity *e = scope_cast<Scene::Entity::Entity>(scope);
			if (e) {
				e->init();
			}
		}
    }

    Scene::SceneManager::getSingleton().onLoad();

	if (in.process())
		in.process()->endSubProcess();

}

}
} // namespace Scripting
} // namespace Core

