#pragma once

#include "../uniquecomponent/uniquecomponentdefine.h"

#include "globalapibase.h"

#include "Interfaces/scripting/types/scope.h"

DEFINE_UNIQUE_COMPONENT(Engine::App, GlobalAPIBase, Application&, GlobalAPI, MADGINE_BASE);

namespace Engine
{
	namespace App
	{
		template <class T>
		using GlobalAPI = Scripting::Scope<T, GlobalAPIComponent<T>>;
	}
}
