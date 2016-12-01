#include "madginelib.h"
#include "struct.h"
#include "Scripting/scriptingexception.h"

#include "Database/exceptionmessages.h"

namespace Engine {
namespace Scripting {

Struct::Factory Struct::sFactory;

template <> Scope *Struct::Factory::create(Serialize::SerializeInStream &in)
{
    return OGRE_NEW Struct();
}

} // namespace Scripting
}

