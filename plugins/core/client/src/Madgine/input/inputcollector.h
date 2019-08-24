#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, InputHandler, MADGINE_CLIENT, InputHandler, Engine::Window::Window *, Engine::App::Application &, Engine::Input::InputListener *);
