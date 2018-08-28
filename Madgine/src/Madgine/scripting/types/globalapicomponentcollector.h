#pragma once


#include "../../uniquecomponentcollector.h"

//#include "globalapicomponentbase.h"

namespace Engine
{

	namespace Scripting
	{
		using GlobalAPICollector = UniqueComponentCollector<GlobalAPIComponentBase, App::Application &>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT class UniqueComponentCollector<GlobalAPIComponentBase, App::Application&>;
#endif

		using GlobalAPICollectorInstance = UniqueComponentCollectorInstance<GlobalAPIComponentBase, std::vector, App::Application&>;

	}

	PLUGIN_COLLECTOR_EXPORT(GlobalAPI, Scripting::GlobalAPICollector);

}
