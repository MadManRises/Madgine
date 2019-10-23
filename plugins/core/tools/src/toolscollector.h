#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "toolbase.h"

#include "Modules/serialize/serializableunit.h"

DECLARE_UNIQUE_COMPONENT(Engine::Tools, Tools, ToolBase, ImRoot &);

namespace Engine {
namespace Tools {

	template <typename T>
	using Tool = Serialize::SerializableUnit<T, ToolsComponent<T>>;

}
}