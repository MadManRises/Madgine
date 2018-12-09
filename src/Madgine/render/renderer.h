#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"
#include "../uniquecomponent/uniquecomponent.h"

#include "rendererbase.h"

namespace Engine
{

	namespace Render
	{
		using RendererCollector = UniqueComponentCollector<RendererBase, GUI::GUISystem *>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_CLIENT_EXPORT struct UniqueComponentCollector<RendererBase, App::ClientApplication&>;
#endif

		using RendererSelector = UniqueComponentSelector<RendererBase, GUI::GUISystem *>;

#ifndef STATIC_BUILD
		template <class T>
		using Renderer = UniqueComponent<T, RendererCollector>;
#else		
		template <class T>
		struct Renderer : RendererBase { using RendererBase::RendererBase; };
#endif		
		
	}

}

RegisterCollector(Engine::Render::RendererCollector);