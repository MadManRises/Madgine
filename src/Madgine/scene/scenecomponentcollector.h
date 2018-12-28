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
	template struct MADGINE_BASE_EXPORT UniqueComponentCollector<Scene::SceneComponentBase, Scene::SceneManager&>::Registry;
#endif

}

RegisterCollector(Engine::Scene::SceneComponentCollector);
