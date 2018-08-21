#pragma once


#include "../../uniquecomponentcollector.h"

namespace Engine
{

	namespace Scripting
	{
		using GlobalAPICollector = BaseUniqueComponentCollector<GlobalAPIComponentBase, std::vector, App::Application &>;
	}

	PLUGIN_COLLECTOR_EXPORT(GlobalAPI, Scripting::GlobalAPICollector);

}
