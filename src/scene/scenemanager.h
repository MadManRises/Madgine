#pragma once


#include "../serialize/serializableunit.h"
#include "scenemanagerbase.h"


namespace Engine
{
	namespace Scene
	{

		template <class T>
		using SceneManager = Serialize::SerializableUnit<T, SceneManagerBase>;
	}
}
