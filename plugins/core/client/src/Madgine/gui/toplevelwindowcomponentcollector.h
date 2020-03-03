#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "toplevelwindowcomponent.h"

DECLARE_UNIQUE_COMPONENT(Engine::GUI, TopLevelWindowComponent, TopLevelWindowComponentBase, TopLevelWindow &);


namespace Engine {
namespace GUI {

    template <typename T>
    using TopLevelWindowComponent = VirtualScope<T, Serialize::SerializableUnit<T, TopLevelWindowComponentComponent<T>>>;

	template <typename T>
    using TopLevelWindowVirtualBase = Serialize::SerializableUnit<T, TopLevelWindowComponentVirtualBase<T>>;

}
}
