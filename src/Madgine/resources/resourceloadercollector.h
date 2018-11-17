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

}

RegisterCollector(Engine::Resources::ResourceLoaderCollector);
