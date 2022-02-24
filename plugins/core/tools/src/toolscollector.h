#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/serialize/hierarchy/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Tools, Tools, ToolBase, ImRoot &);

namespace Engine {
namespace Tools {

	template <typename T>
	using Tool = Serialize::VirtualUnit<T, VirtualScope<T, ToolsComponent<T>>>;

}
}