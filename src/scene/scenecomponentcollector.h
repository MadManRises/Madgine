#pragma once


#include "../uniquecomponentcollector.h"

#include "scenecomponentset.h"

#include "scenecomponentbase.h"

namespace Engine
{
	namespace Scene
	{

		using SceneComponentCollector = BaseUniqueComponentCollector<SceneComponentBase, SceneComponentSet, SceneManagerBase&>;

	}

}
