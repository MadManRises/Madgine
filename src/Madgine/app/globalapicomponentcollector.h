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
		MADGINE_BASE_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<App::GlobalAPIComponentBase, App::Application&>;
#endif

}

RegisterCollector(Engine::App::GlobalAPICollector);