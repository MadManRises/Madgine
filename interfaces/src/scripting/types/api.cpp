#include "interfaceslib.h"
#include "api.h"

namespace Engine {

	ValueType toValueType(Scripting::ScopeBase *ref, const Scripting::Mapper &mapper) {
		return mapper.mToValueType ? mapper.mToValueType(ref) : ValueType{};
	}

}