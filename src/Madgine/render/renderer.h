#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"
#include "../uniquecomponent/uniquecomponent.h"

#include "rendererbase.h"

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
	template struct MADGINE_CLIENT_EXPORT UniqueComponentRegistry<Render::RendererBase, GUI::GUISystem *>;
#endif

}

RegisterCollector(Engine::Render::RendererCollector);