#include "interfaceslib.h"
#include "prototype.h"

namespace Engine {

	namespace Scripting {
		
		std::string Prototype::getIdentifier()
		{
			return typeid(Prototype).name();
		}

	} // namespace Scripting

}

