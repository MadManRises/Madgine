#pragma once

#include "scenecomponentcollector.h"

#include "../uniquecomponent.h"
#include "scenecomponentbase.h"
#include "../serialize/serializableunit.h"
#include "../scripting/types/scope.h"

namespace Engine
{
	namespace Scene
	{

		template <class T>
		using SceneComponent = Serialize::SerializableUnit<
			T, Scripting::Scope<T, UniqueComponent<T, SceneComponentCollector>>>;
	}

}
