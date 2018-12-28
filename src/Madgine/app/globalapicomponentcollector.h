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
		template struct MADGINE_BASE_EXPORT UniqueComponentCollector<App::GlobalAPIComponentBase, App::Application&>::Registry;
#endif

}

RegisterCollector(Engine::App::GlobalAPICollector);