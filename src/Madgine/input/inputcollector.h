#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"

#include "inputhandler.h"

namespace Engine
{

	namespace Input
	{
		using InputCollector = UniqueComponentCollector<InputHandler, Window::Window*>;

		using InputSelector = UniqueComponentSelector<InputHandler, Window::Window*>;

	}

#ifndef STATIC_BUILD
	MADGINE_CLIENT_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<Input::InputHandler, Window::Window*>;
#endif

}

RegisterCollector(Engine::Input::InputCollector);