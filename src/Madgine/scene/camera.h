#pragma once

#include "Modules/math/vector3.h"
#include "Modules/math/vector4.h"
#include "Modules/math/quaternion.h"
#include "Modules/keyvalue/scopebase.h"

namespace Engine
{
	namespace Scene
	{
		
		class MADGINE_BASE_EXPORT Camera : public ScopeBase
		{
		public:
			Camera();
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

			//KeyValueMapList maps() override;

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