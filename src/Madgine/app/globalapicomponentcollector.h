#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"

#include "globalapicomponentbase.h"

namespace Engine
{

	namespace App
	{
		using GlobalAPICollector = UniqueComponentCollector<GlobalAPIComponentBase, Application &>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT struct UniqueComponentCollector<GlobalAPIComponentBase, Application&>;
#endif

		using GlobalAPIContainer = UniqueComponentContainer<GlobalAPIComponentBase, Application&>;

	}

}

RegisterCollector(Engine::App::GlobalAPICollector);