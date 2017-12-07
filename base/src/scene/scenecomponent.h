#pragma once

#include "uniquecomponent.h"
#include "scenecomponentbase.h"
#include "serialize/serializableunit.h"
#include "scripting/types/scope.h"


#include "uniquecomponentcollector.h"

#include "scenecomponentset.h"

namespace Engine
{
	namespace Scene
	{

		using SceneComponentCollector = BaseUniqueComponentCollector<SceneComponentBase, SceneComponentSet, SceneManagerBase*>;

		template <class T>
		using SceneComponent = Serialize::SerializableUnit<
			T, Scripting::Scope<T, UniqueComponent<T, SceneComponentCollector>>>;
	}

}
