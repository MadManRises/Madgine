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
	template struct MADGINE_CLIENT_EXPORT UniqueComponentCollector<Input::InputHandler, Window::Window*>;
#endif

}

RegisterCollector(Engine::Input::InputCollector);