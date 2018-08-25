#pragma once


#include "../../uniquecomponentcollector.h"

//#include "globalapicomponentbase.h"

namespace Engine
{

	namespace Scripting
	{
		using GlobalAPICollector = UniqueComponentCollector<GlobalAPIComponentBase, std::vector, App::Application &>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT class UniqueComponentCollector<GlobalAPIComponentBase, std::vector, App::Application&>;
#endif

	}

	PLUGIN_COLLECTOR_EXPORT(GlobalAPI, Scripting::GlobalAPICollector);

}
