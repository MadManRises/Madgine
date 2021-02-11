#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/serialize/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Tools, Tools, ToolBase, ImRoot &);

namespace Engine {
namespace Tools {

	template <typename T>
	using Tool = Serialize::VirtualUnit<T, VirtualScope<T, ToolsComponent<T>>>;

}
}