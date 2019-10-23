#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "renderwindow.h"

DECLARE_UNIQUE_COMPONENT(Engine::Render, RenderWindow, RenderWindow, Window::Window *, GUI::TopLevelWindow *, Engine::Render::RenderWindow *);
