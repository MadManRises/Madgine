#pragma once


#include "../uniquecomponentcollector.h"

#include "scenecomponentbase.h"


namespace Engine
{
	namespace Scene
	{

		template <class T>
		class SceneComponentSet;

		using SceneComponentCollector = UniqueComponentCollector<SceneComponentBase, SceneManager&>;

#ifndef PLUGIN_BUILD
		extern template MADGINE_BASE_EXPORT struct UniqueComponentCollector<SceneComponentBase, SceneManager&>;
#endif

		using SceneComponentCollectorInstance = UniqueComponentCollectorInstance<SceneComponentBase, SceneManager&, SceneComponentSet>;

	}

}

RegisterCollector(Engine::Scene::SceneComponentCollector);
