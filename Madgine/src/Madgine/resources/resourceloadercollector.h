#pragma once


#include "../uniquecomponentcollector.h"

#include "resourceloaderbase.h"

namespace Engine
{
	namespace Resources
	{

		using ResourceLoaderCollector = BaseUniqueComponentCollector<ResourceLoaderBase, std::vector, ResourceManager&>;

	}

	PLUGIN_COLLECTOR_EXPORT(ResourceLoader, Resources::ResourceLoaderCollector);

}
