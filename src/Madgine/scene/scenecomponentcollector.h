#pragma once


#include "../uniquecomponent/uniquecomponentcollector.h"

#include "scenecomponentbase.h"


namespace Engine
{
	namespace Scene
	{

		template <class T>
		class SceneComponentSet;

		using SceneComponentCollector = UniqueComponentCollector<SceneComponentBase, SceneManager&>;

		using SceneComponentContainer = UniqueComponentContainer<SceneComponentBase, SceneManager&, SceneComponentSet>;

	}

#ifndef STATIC_BUILD
	MADGINE_BASE_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<Scene::SceneComponentBase, Scene::SceneManager&>;
#endif

}

RegisterCollector(Engine::Scene::SceneComponentCollector);
