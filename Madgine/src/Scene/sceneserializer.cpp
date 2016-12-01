#include "madginelib.h"
#include "sceneserializer.h"

#include "Scripting/Types/globalscope.h"
#include "SceneManager.h"
#include "Entity/entity.h"

#include "Scripting/Types/scopefactory.h"
#include "Scripting/Datatypes/Serialize/serializestream.h"

#include "UI/Process.h"

namespace Engine {
namespace Scene {

void SceneSerializer::storeCurrentScene(Scripting::Serialize::SerializeOutStream &out, bool storeComponents, const std::set<Scripting::Scope*> &ignoreSet)
{

	Scripting::GlobalScope *global = &Scripting::GlobalScope::getSingleton();

    out << Scene::SceneManager::getSingleton();

    if (storeComponents)
        Scene::SceneManager::getSingleton().saveComponentData(out);
    else
        out << Scripting::ValueType();


    std::set<Scripting::Scope *> scopeSet;

    global->collectScopes(scopeSet, ignoreSet);
	
	for (Scene::Entity::Entity *e : Scene::SceneManager::getSingleton().entities()) {
		e->collectScopes(scopeSet, ignoreSet);
	}

    global->level()->collectScopes(scopeSet, ignoreSet);

    for (Scripting::Scope *s : scopeSet){
        out << s;
        s->storeCreationData(out);
    }

    out << Scripting::ValueType();


    for (Scripting::Scope *s : scopeSet){
        out << s << *s;
    }

    out << Scripting::ValueType();
}

void SceneSerializer::restoreScene(Scripting::Serialize::SerializeInStream &in, bool callInit)
{

	if (in.process())
		in.process()->startSubProcess(1, "Loading Level...");

    in >> Scene::SceneManager::getSingleton();

    Scene::SceneManager::getSingleton().loadComponentData(in);

    std::map<Scripting::InvScopePtr, Scripting::Scope *> scopeMap;

    Scripting::ValueType scopeIn;

	Scripting::ScopeFactoryManager *factory = &Scripting::ScopeFactoryManager::getSingleton();
	Scripting::InvScopePtr oldPtr;
    while(in.loopRead(oldPtr)){
		Scripting::Scope *scope = factory->create(in);
		if (!scope) return;
        scopeMap[oldPtr] = scope;

    }

    while(in.loopRead(oldPtr)){
        auto it = scopeMap.find(oldPtr);
        if (it == scopeMap.end()) throw 0;
        Scripting::Scope *scope = it->second;
        in >> *scope;
        scope->applyScopeMap(scopeMap);

		if (callInit) {
			Scene::Entity::Entity *e = Scripting::scope_cast<Scene::Entity::Entity>(scope);
			if (e) {
				e->init();
			}
		}
    }

    Scene::SceneManager::getSingleton().onLoad();

	if (in.process())
		in.process()->endSubProcess();

}

} // namespace Scripting
} // namespace Core

