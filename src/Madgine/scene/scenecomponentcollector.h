#pragma once


#include "../uniquecomponent/uniquecomponentdefine.h"

#include "scenecomponentbase.h"
#include "Interfaces/serialize/serializableunit.h"
#include "Interfaces/scripting/types/scope.h"

DEFINE_UNIQUE_COMPONENT(Engine::Scene, SceneComponentBase, SceneManager&, SceneComponent, MADGINE_BASE);

namespace Engine
{
	namespace Scene
	{

		template <class T>
		using SceneComponent = Serialize::SerializableUnit <
			T, Scripting::Scope<T, SceneComponentComponent<T>>>;

		template <class T>
		using VirtualSceneComponentBase = Serialize::SerializableUnit<T, Scripting::Scope<T, SceneComponentVirtualBase<T>>>;

		template <class T, class Base>
		using VirtualSceneComponentImpl = Serialize::SerializableUnit<T, VirtualUniqueComponentImpl<T, Base>>;

#define VIRTUALSCENECOMPONENTBASE(T) template <> TEMPLATE_INSTANCE constexpr size_t &Engine::Scene::VirtualSceneComponentBase<T>::sIndex(){static size_t index = -1; return index;};

	}
}
