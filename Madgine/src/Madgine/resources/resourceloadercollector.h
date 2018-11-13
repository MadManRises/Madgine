#pragma once


#include "../uniquecomponentcollector.h"

#include "resourceloaderbase.h"

namespace Engine
{
	namespace Resources
	{

		using ResourceLoaderCollector = UniqueComponentCollector<ResourceLoaderBase, ResourceManager&>;

		using ResourceLoaderCollectorInstance = UniqueComponentCollectorInstance<ResourceLoaderBase, ResourceManager &>;

	}

}

RegisterCollector(Engine::Resources::ResourceLoaderCollector);
