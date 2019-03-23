#pragma once

#include "../entitycomponent.h"
#include "Interfaces/serialize/container/serializedmapper.h"
#include "Interfaces/math/vector3.h"
#include "Interfaces/math/matrix4.h"
#include "Interfaces/math/quaternion.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			
			class MADGINE_BASE_EXPORT Transform : public EntityComponent<Transform>
			{
			public:
				using EntityComponent<Transform>::EntityComponent;

				const Vector3 &getPosition() const;
				const Vector3 &getScale() const;
				const Quaternion &getOrientation() const;

				void setPosition(const Vector3 &position);
				void setScale(const Vector3 &scale);
				void setOrientation(const Quaternion &orientation);

				void translate(const Vector3 &v);
				void rotate(const Quaternion &q);

				KeyValueMapList maps() override;

				Matrix4 matrix() const;

			private:
				Vector3 mPosition;
				Vector3 mScale = Vector3::UNIT_SCALE;
				Quaternion mOrientation;
			};

		}
	}
}

RegisterType(Engine::Scene::Entity::Transform);