#pragma once


#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Meta/serialize/hierarchy/syncableunit.h"

#include "Meta/keyvalue/virtualscope.h"

DECLARE_UNIQUE_COMPONENT(Engine::Scene, SceneComponent, SceneComponentBase, SceneManager &);

namespace Engine
{
	namespace Scene
	{

		template <typename T>
		using SceneComponent = VirtualScope<T, Serialize::SyncableUnit<T, SceneComponentComponent<T>>>;

		template <typename T>
		using VirtualSceneComponentBase = Serialize::SyncableUnit<T, SceneComponentVirtualBase<T>>;

		template <typename T, typename Base>
		using VirtualSceneComponentImpl = VirtualScope<T, Serialize::SyncableUnit<T, VirtualUniqueComponentImpl<T, Base>>>;

#define VIRTUALSCENECOMPONENTBASE(T) template <> TEMPLATE_INSTANCE constexpr size_t &Engine::Scene::VirtualSceneComponentBase<T>::sIndex(){static size_t index = -1; return index;};

	}
}
