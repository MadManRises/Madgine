#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, InputHandler, InputHandler, Engine::GUI::TopLevelWindow &, Engine::Window::Window *, Engine::Input::InputListener *);
