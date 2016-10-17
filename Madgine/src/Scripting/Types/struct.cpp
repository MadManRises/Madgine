#include "libinclude.h"
#include "struct.h"
#include "Scripting/scriptexception.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {

Struct::Factory Struct::sFactory;

Struct::Struct()
{

}


std::string Struct::getIdentifier()
{
    return "Struct";
}

Scope *Struct::Factory::create(Serialize::SerializeInStream &in)
{
    return OGRE_NEW Struct();
}

} // namespace Scripting
}

