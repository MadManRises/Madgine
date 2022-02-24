#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/serialize/hierarchy/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Window, MainWindowComponent, MainWindowComponentBase, MainWindow &);


namespace Engine {
namespace Window {

    template <typename T>
    using MainWindowComponent = Serialize::VirtualData<T, VirtualScope<T, MainWindowComponentComponent<T>>>;

	template <typename T>
    using MainWindowVirtualBase = Serialize::VirtualData<T, MainWindowComponentVirtualBase<T>>;

    template <typename T, typename Base>
    using MainWindowVirtualImpl = VirtualUniqueComponentImpl<T, Base>;

}
}
