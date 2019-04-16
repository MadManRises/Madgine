#pragma once

#include "Interfaces/math/vector3.h"
#include "Interfaces/math/vector4.h"
#include "Interfaces/math/quaternion.h"
#include "Interfaces/scripting/types/scope.h"

namespace Engine
{
	namespace Scene
	{
		
		class MADGINE_BASE_EXPORT Camera : public Scripting::Scope<Camera>
		{
		public:
			Camera(SceneManager &scene);
			~Camera() = default;

			void setPosition(const Vector3 &pos);
			const Vector3 &position();
			void setOrientation(const Quaternion &q);
			const Quaternion &orientation();

			Matrix4 getViewProjectionMatrix(float aspectRatio);

			float getF() const;
			void setF(float f);
			float getN() const;
			void setN(float n);
			float getFOV() const;
			void setFOV(float fov);

			KeyValueMapList maps() override;

			Vector3 getOrientationHandle() const;
			void rotate(const Vector3 &rot);

			const std::vector<Entity::Entity*> &visibleEntities() const;
			void setVisibleEntities(std::vector<Entity::Entity*> &&entities);

		private:
			Vector3 mPosition;
			Quaternion mOrientation;

			float mF;
			float mN;
			float mFOV;

			std::vector<Entity::Entity*> mVisibleEntities;

		};

	}
}

RegisterType(Engine::Scene::Camera);