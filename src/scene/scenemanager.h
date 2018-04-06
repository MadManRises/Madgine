#pragma once


#include "../serialize/serializableunit.h"
#include "scenemanagerbase.h"


namespace Engine
{
	namespace Scene
	{

		template <class T>
		class SceneManager : public Singleton<T>, public Serialize::SerializableUnit<T, SceneManagerBase>
		{
		public:
			using Serialize::SerializableUnit<T, SceneManagerBase>::SerializableUnit;
		};
	}
}
