#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "rendererbase.h"

DECLARE_UNIQUE_COMPONENT(Engine::Render, Renderer, MADGINE_CLIENT, RendererBase, GUI::GUISystem *);
