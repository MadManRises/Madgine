#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "renderwindow.h"

DECLARE_UNIQUE_COMPONENT(Engine::Render, RenderWindow, MADGINE_CLIENT, RenderWindow, Window::Window *, GUI::TopLevelWindow *, Engine::Render::RenderWindow *);
