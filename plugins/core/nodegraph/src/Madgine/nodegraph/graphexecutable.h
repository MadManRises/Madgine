#pragma once

#include "Modules/serialize/serializableunit.h"

namespace Engine {
namespace NodeGraph {

	struct GraphExecutable : Serialize::SerializableUnitBase{
        virtual void execute(NodeInstance *instance, size_t flowIndex, ArgumentList *out = nullptr) const = 0;
	};

}
}