#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "scenecomponentbase.h"
#include "Modules/serialize/serializableunit.h"


DECLARE_UNIQUE_COMPONENT(Engine::Scene, SceneComponent, MADGINE_BASE, SceneComponentBase, SceneManager &);

namespace Engine
{
	namespace Scene
	{

		template <class T>
		using SceneComponent = Serialize::SerializableUnit<T, SceneComponentComponent<T>>;

		template <class T>
		using VirtualSceneComponentBase = Serialize::SerializableUnit<T, SceneComponentVirtualBase<T>>;

		template <class T, class Base>
		using VirtualSceneComponentImpl = Serialize::SerializableUnit<T, VirtualUniqueComponentImpl<T, Base>>;

#define VIRTUALSCENECOMPONENTBASE(T) template <> TEMPLATE_INSTANCE constexpr size_t &Engine::Scene::VirtualSceneComponentBase<T>::sIndex(){static size_t index = -1; return index;};

	}
}
