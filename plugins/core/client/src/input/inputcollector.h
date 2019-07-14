#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

using InputTuple = std::tuple<Engine::Window::Window*, Engine::App::Application&, Engine::Input::InputListener *>;

DECLARE_UNIQUE_COMPONENT(Engine::Input, InputHandler, InputTuple, InputHandler, MADGINE_CLIENT);
