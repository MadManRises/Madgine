#pragma once


#include "../uniquecomponentcollector.h"


namespace Engine
{
	namespace Scene
	{

		template <class T>
		class SceneComponentSet;

		using SceneComponentCollector = UniqueComponentCollector<SceneComponentBase, SceneManager&>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT class UniqueComponentCollector<SceneComponentBase, SceneManager&>;
#endif

		using SceneComponentCollectorInstance = UniqueComponentCollectorInstance<SceneComponentBase, SceneComponentSet, SceneManager&>;

	}

	PLUGIN_COLLECTOR_EXPORT(SceneComponent, Scene::SceneComponentCollector);

}
