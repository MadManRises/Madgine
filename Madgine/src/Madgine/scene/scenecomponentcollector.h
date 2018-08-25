#pragma once


#include "../uniquecomponentcollector.h"

#include "scenecomponentset.h"

namespace Engine
{
	namespace Scene
	{

		using SceneComponentCollector = UniqueComponentCollector<SceneComponentBase, SceneComponentSet, SceneManager&>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT class UniqueComponentCollector<SceneComponentBase, SceneComponentSet, SceneManager&>;
#endif

	}

	PLUGIN_COLLECTOR_EXPORT(SceneComponent, Scene::SceneComponentCollector);

}
