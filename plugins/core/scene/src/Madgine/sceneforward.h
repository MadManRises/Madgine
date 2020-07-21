#pragma once

namespace Engine
{
	namespace Scene
	{
		struct SceneManager;
		struct SceneComponentBase;

		

		namespace Entity
		{
			struct Entity;
			struct EntityComponentBase;
			struct Transform;
			struct Animation;
			struct Mesh;

			struct EntityPtr;
			template <typename T>
			using EntityComponentPtr = EntityPtr;
		}

		struct LightManager;
		struct Light;
	}

}
