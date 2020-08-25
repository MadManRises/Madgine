#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

DECLARE_UNIQUE_COMPONENT(Engine::Input, InputHandler, InputHandler, Engine::Window::MainWindow &, Engine::Window::OSWindow *, Engine::Input::InputListener *);
