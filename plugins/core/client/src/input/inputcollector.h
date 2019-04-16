#pragma once


#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"

#include "inputhandler.h"

DEFINE_UNIQUE_COMPONENT(Engine::Input, InputHandler, Window::Window*, InputHandler, MADGINE_CLIENT);
