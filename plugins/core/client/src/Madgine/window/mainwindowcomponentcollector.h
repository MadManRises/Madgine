#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/serialize/serializableunit.h"

DECLARE_UNIQUE_COMPONENT(Engine::Window, MainWindowComponent, MainWindowComponentBase, MainWindow &);


namespace Engine {
namespace Window {

    template <typename T>
    using MainWindowComponent = VirtualScope<T, Serialize::SerializableUnit<T, MainWindowComponentComponent<T>>>;

	template <typename T>
    using MainWindowVirtualBase = Serialize::SerializableUnit<T, MainWindowComponentVirtualBase<T>>;

}
}
