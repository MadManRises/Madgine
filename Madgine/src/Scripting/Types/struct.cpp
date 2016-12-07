#include "madginelib.h"
#include "struct.h"
#include "Scripting/scriptingexception.h"

#include "Database/exceptionmessages.h"

namespace Engine {
	
	namespace Scripting {

std::string Struct::getIdentifier()
{
	return typeid(Struct).name();
}

} // namespace Scripting

}

