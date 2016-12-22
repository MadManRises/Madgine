#include "madginelib.h"
#include "scene.h"
#include "globalscope.h"

namespace Engine {
namespace Scripting {



Scene::Scene()
{

}

std::string Scene::getIdentifier()
{
	return typeid(Scene).name();
}

} // namespace Scripting


} // namespace Core

