#include "madginelib.h"
#include "scene.h"
#include "globalscope.h"

namespace Engine {
namespace Scripting {

Scene::Factory Scene::sFactory;

Scene::Scene()
{

}


void Scene::clear()
{
	Scope::clear();
}



template <> Scope *Scene::Factory::create(Serialize::SerializeInStream &in)
{
    return GlobalScope::getSingleton().level();
}

} // namespace Scripting
} // namespace Core

