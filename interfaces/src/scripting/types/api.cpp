#include "interfaceslib.h"
#include "api.h"

namespace Engine {
	namespace Scripting {

		ValueType toValueType(Scripting::ScopeBase *ref, const Scripting::Mapper &mapper) {
			return mapper.mToValueType ? mapper.mToValueType(ref) : ValueType{};
		}

	}
}