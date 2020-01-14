#pragma once

#include "../entitycomponent.h"
#include "Modules/math/vector3.h"
#include "Modules/math/matrix4.h"
#include "Modules/math/quaternion.h"

#include "Modules/serialize/container/serialized.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			
			struct MADGINE_SCENE_EXPORT Transform : EntityComponent<Transform>
			{			
				using EntityComponent<Transform>::EntityComponent;

				const Vector3 &getPosition() const;
				const Vector3 &getScale() const;
				const Quaternion &getOrientation() const;

				void setPosition(const Vector3 &position);
				void setScale(const Vector3 &scale);
				void setOrientation(const Quaternion &orientation);

				void translate(const Vector3 &v);
				void rotate(const Quaternion &q);

				//KeyValueMapList maps() override;

				Matrix4 matrix() const;

			private:
				Serialize::Serialized<Vector3> mPosition;
				Vector3 mScale = Vector3::UNIT_SCALE;
				Quaternion mOrientation;
			};

		}
	}
}