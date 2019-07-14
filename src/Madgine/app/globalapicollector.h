#pragma once

#include "Modules/uniquecomponent/uniquecomponentdefine.h"
#include "globalapibase.h"

DECLARE_UNIQUE_COMPONENT(Engine::App, GlobalAPIBase, Application&, GlobalAPI, MADGINE_BASE);

namespace Engine
{
	namespace App
	{
		template <class T>
		using GlobalAPI = GlobalAPIComponent<T>;
	}
}
