#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, InputHandler, InputHandler, Engine::Window::MainWindow &, Engine::Window::Window *, Engine::Input::InputListener *);
