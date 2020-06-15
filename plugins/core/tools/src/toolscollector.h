#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/keyvalue/virtualscopebase.h"

DECLARE_UNIQUE_COMPONENT(Engine::Tools, Tools, ToolBase, ImRoot &);

namespace Engine {
namespace Tools {

	template <typename T>
	using Tool = Serialize::SerializableUnit<T, VirtualScope<T, ToolsComponent<T>>>;

}
}