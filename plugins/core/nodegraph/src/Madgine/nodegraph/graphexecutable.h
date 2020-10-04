#pragma once

#include "Modules/serialize/syncableunit.h"

namespace Engine {
namespace NodeGraph {

	struct GraphExecutable : Serialize::SyncableUnitBase{
        virtual void execute(NodeInstance *instance, uint32_t flowIndex, ArgumentList *out = nullptr) const = 0;
	};

}
}