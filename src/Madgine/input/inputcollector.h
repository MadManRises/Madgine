#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"

#include "inputhandler.h"

namespace Engine
{

	namespace Input
	{
		using InputCollector = UniqueComponentCollector<InputHandler, Window::Window*>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_CLIENT_EXPORT struct UniqueComponentCollector<InputHandler, Window::Window*>;
#endif

		using InputSelector = UniqueComponentSelector<InputHandler, Window::Window*>;

	}

}

RegisterCollector(Engine::Input::InputCollector);