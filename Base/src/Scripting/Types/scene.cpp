#include "baselib.h"
#include "scene.h"
#include "GlobalScope.h"

namespace Engine {
namespace Scripting {

std::string Scene::getIdentifier()
{
	return typeid(Scene).name();
}

} // namespace Scripting


} // namespace Core

