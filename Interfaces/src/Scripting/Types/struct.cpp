#include "interfaceslib.h"
#include "struct.h"


namespace Engine {
	
	namespace Scripting {

std::string Struct::getIdentifier()
{
	return typeid(Struct).name();
}

} // namespace Scripting

}

