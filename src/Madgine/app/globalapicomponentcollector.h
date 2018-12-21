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

}

RegisterCollector(Engine::App::GlobalAPICollector);