#include "libinclude.h"
#include "level.h"
#include "globalscope.h"

namespace Engine {
namespace Scripting {

Level::Factory Level::sFactory;

Level::Level()
{

}


void Level::clear()
{
	Scope::clear();
}


std::string Level::getIdentifier()
{
    return "Level";
}

Scope *Level::Factory::create(Serialize::SerializeInStream &in)
{
    return GlobalScope::getSingleton().level();
}

} // namespace Scripting
} // namespace Core

