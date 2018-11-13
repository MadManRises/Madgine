#pragma once

#include "scenecomponentcollector.h"

#include "../uniquecomponent.h"
#include "scenecomponentbase.h"
#include "Interfaces/serialize/serializableunit.h"
#include "Interfaces/scripting/types/scope.h"

namespace Engine
{
	namespace Scene
	{

		template <class T>
		using SceneComponent = Serialize::SerializableUnit<
			T, Scripting::Scope<T, UniqueComponent<T, SceneComponentCollector>>>;

		template <class T>
		using VirtualSceneComponentBase = Serialize::SerializableUnit<T, Scripting::Scope<T, VirtualUniqueComponentBase<T, SceneComponentCollector>>>;

		template <class T, class Base>
		using VirtualSceneComponentImpl = Serialize::SerializableUnit<T, VirtualUniqueComponentImpl<T, Base>>;

#define VIRTUALSCENECOMPONENTBASE(T) template <> TEMPLATE_INSTANCE constexpr size_t &Engine::Scene::VirtualSceneComponentBase<T>::sIndex(){static size_t index = -1; return index;};

	}

}
