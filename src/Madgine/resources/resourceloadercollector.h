#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"

#include "resourceloaderbase.h"

namespace Engine
{
	namespace Resources
	{

		using ResourceLoaderCollector = UniqueComponentCollector<ResourceLoaderBase, ResourceManager&>;

		using ResourceLoaderContainer = UniqueComponentContainer<ResourceLoaderBase, ResourceManager &>;

	}


#ifndef STATIC_BUILD
	template struct MADGINE_BASE_EXPORT UniqueComponentRegistry<Resources::ResourceLoaderBase, Resources::ResourceManager&>;
#endif


}

RegisterCollector(Engine::Resources::ResourceLoaderCollector);
