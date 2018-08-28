#pragma once


#include "../uniquecomponentcollector.h"

#include "resourceloaderbase.h"

namespace Engine
{
	namespace Resources
	{

		using ResourceLoaderCollector = UniqueComponentCollector<ResourceLoaderBase, ResourceManager&>;

		using ResourceLoaderCollectorInstance = UniqueComponentCollectorInstance<ResourceLoaderBase, std::vector, ResourceManager &>;

	}

	PLUGIN_COLLECTOR_EXPORT(ResourceLoader, Resources::ResourceLoaderCollector);

}
