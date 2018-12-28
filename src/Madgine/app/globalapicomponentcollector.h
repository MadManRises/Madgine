#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"

#include "globalapicomponentbase.h"

namespace Engine
{

	namespace App
	{
		using GlobalAPICollector = UniqueComponentCollector<GlobalAPIComponentBase, Application &>;


		using GlobalAPIContainer = UniqueComponentContainer<GlobalAPIComponentBase, Application&>;
	}

#ifndef STATIC_BUILD
		template struct MADGINE_BASE_EXPORT UniqueComponentRegistry<App::GlobalAPIComponentBase, App::Application&>;
#endif

}

RegisterCollector(Engine::App::GlobalAPICollector);