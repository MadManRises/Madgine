#pragma once


#include "../uniquecomponentcollector.h"

#include "globalapicomponentbase.h"

namespace Engine
{

	namespace App
	{
		using GlobalAPICollector = UniqueComponentCollector<GlobalAPIComponentBase, Application &>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT struct UniqueComponentCollector<GlobalAPIComponentBase, Application&>;
#endif

		using GlobalAPICollectorInstance = UniqueComponentCollectorInstance<GlobalAPIComponentBase, Application&>;

	}

}

RegisterCollector(Engine::App::GlobalAPICollector);