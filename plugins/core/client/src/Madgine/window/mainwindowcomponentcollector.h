#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/serialize/virtualserializableunit.h"

DECLARE_UNIQUE_COMPONENT(Engine::Window, MainWindowComponent, MainWindowComponentBase, MainWindow &);


namespace Engine {
namespace Window {

    template <typename T>
    using MainWindowComponent = Serialize::VirtualUnit<T, VirtualScope<T, MainWindowComponentComponent<T>>>;

	template <typename T>
    using MainWindowVirtualBase = Serialize::VirtualUnit<T, MainWindowComponentVirtualBase<T>>;

    template <typename T, typename Base>
    using MainWindowVirtualImpl = VirtualUniqueComponentImpl<T, Base>;

}
}
