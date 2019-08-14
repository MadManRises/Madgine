#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, InputHandler, InputHandler, MADGINE_CLIENT, Engine::Window::Window *, Engine::App::Application &, Engine::Input::InputListener *);
