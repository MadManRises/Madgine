#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "rendererbase.h"

DECLARE_UNIQUE_COMPONENT(Engine::Render, RendererBase, Renderer, MADGINE_CLIENT, GUI::GUISystem *);
