#pragma once


#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"

#include "rendererbase.h"

DEFINE_UNIQUE_COMPONENT(Engine::Render, RendererBase, GUI::GUISystem*, Renderer, MADGINE_CLIENT);
