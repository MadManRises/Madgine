#pragma once


#include "../uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

DEFINE_UNIQUE_COMPONENT(Engine::Input, InputHandler, Window::Window*, InputHandler, MADGINE_CLIENT);

/*
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

RegisterCollector(Engine::Input::InputCollector);*/