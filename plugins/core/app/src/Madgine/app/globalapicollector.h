#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_UNIQUE_COMPONENT(Engine::App, GlobalAPI, GlobalAPIBase, Application &)

namespace Engine
{
	namespace App
	{
    template <typename T>
		using GlobalAPI = VirtualScope<T, GlobalAPIComponent<T>>;
	}
}
