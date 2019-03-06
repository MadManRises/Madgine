#pragma once


#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"

#include "resourceloaderbase.h"

DEFINE_UNIQUE_COMPONENT(Engine::Resources, ResourceLoaderBase, ResourceManager&, ResourceLoader, MADGINE_BASE);
/*
namespace Engine
{
	namespace Resources
	{

		using ResourceLoaderCollector = UniqueComponentCollector<ResourceLoaderBase, ResourceManager&>;

		using ResourceLoaderContainer = UniqueComponentContainer<ResourceLoaderBase, ResourceManager &>;

	}


#ifndef STATIC_BUILD
	MADGINE_BASE_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<Resources::ResourceLoaderBase, Resources::ResourceManager&>;
#endif


}

RegisterCollector(Engine::Resources::ResourceLoaderCollector);
*/