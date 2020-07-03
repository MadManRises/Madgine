#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/keyvalueutil/virtualscopebase.h"

DECLARE_UNIQUE_COMPONENT(Engine::Scene, SceneComponent, SceneComponentBase, SceneManager &);

namespace Engine
{
	namespace Scene
	{

		template <typename T>
		using SceneComponent = VirtualScope<T, Serialize::SerializableUnit<T, SceneComponentComponent<T>>>;

		template <typename T>
		using VirtualSceneComponentBase = Serialize::SerializableUnit<T, SceneComponentVirtualBase<T>>;

		template <typename T, typename Base>
		using VirtualSceneComponentImpl = VirtualScope<T, Serialize::SerializableUnit<T, VirtualUniqueComponentImpl<T, Base>>>;

#define VIRTUALSCENECOMPONENTBASE(T) template <> TEMPLATE_INSTANCE constexpr size_t &Engine::Scene::VirtualSceneComponentBase<T>::sIndex(){static size_t index = -1; return index;};

	}
}
