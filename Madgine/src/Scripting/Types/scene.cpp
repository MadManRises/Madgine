#include "madginelib.h"
#include "scene.h"
#include "globalscope.h"

namespace Engine {
namespace Scripting {



Scene::Scene(Serialize::TopLevelSerializableUnit *topLevel) :
	Scope(topLevel)
{

}

std::string Scene::getIdentifier()
{
	return typeid(Scene).name();
}

} // namespace Scripting


} // namespace Core

