#pragma once


#include "../uniquecomponent/uniquecomponentdefine.h"

#include "rendererbase.h"

DEFINE_UNIQUE_COMPONENT(Engine::Render, RendererBase, GUI::GUISystem*, Renderer, MADGINE_CLIENT);
/*
namespace Engine
{

	namespace Render
	{
		using RendererCollector = UniqueComponentCollector<RendererBase, GUI::GUISystem *>;


		using RendererSelector = UniqueComponentSelector<RendererBase, GUI::GUISystem *>;

		//Weird compiler issue
#ifndef STATIC_BUILD
		template <class T>
		using Renderer = UniqueComponent<T, RendererCollector>;
#else		
		template <class T>
		struct Renderer : RendererBase { using RendererBase::RendererBase; };
#endif		
		
	}
	
#ifndef STATIC_BUILD
	MADGINE_CLIENT_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<Render::RendererBase, GUI::GUISystem *>;
#endif

}

RegisterCollector(Engine::Render::RendererCollector);
*/