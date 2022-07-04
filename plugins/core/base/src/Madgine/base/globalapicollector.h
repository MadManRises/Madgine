#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_UNIQUE_COMPONENT(Engine::Base, GlobalAPI, GlobalAPIBase, Application &)

namespace Engine
{
	namespace Base
	{
    template <typename T>
		using GlobalAPI = VirtualScope<T, GlobalAPIComponent<T>>;
	}
}
