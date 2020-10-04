#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/serialize/virtualserializableunit.h"

#include "Modules/keyvalueutil/virtualscopebase.h"

DECLARE_UNIQUE_COMPONENT(Engine::Tools, Tools, ToolBase, ImRoot &);

namespace Engine {
namespace Tools {

	template <typename T>
	using Tool = Serialize::VirtualUnit<T, VirtualScope<T, ToolsComponent<T>>>;

}
}