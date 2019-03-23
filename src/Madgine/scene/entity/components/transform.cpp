#include "../../../baselib.h"

#include "transform.h"

#include "Interfaces/scripting/types/api.h"
#include "Interfaces/generic/keyvalueiterate.h"



namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			
			ENTITYCOMPONENT_IMPL(Transform, Transform);

			KeyValueMapList Transform::maps()
			{
				return Scope::maps().merge(MAP(Position, getPosition, setPosition), MAP(Orientation, getOrientation, setOrientation), MAP(Scale, getScale, setScale));
			}

			Matrix4 Transform::matrix() const
			{
				return Matrix4{ mOrientation.toMatrix() } *Matrix4{ mScale.x, 0, 0, mPosition.x,
																	0, mScale.y, 0, mPosition.y,
																	0, 0, mScale.z, mPosition.z,
																	0, 0, 0, 1 };
			}

			const Vector3 &Transform::getPosition() const
			{
				return mPosition;
			}
			const Vector3 &Transform::getScale() const
			{
				return mScale;
			}
			const Quaternion &Transform::getOrientation() const
			{
				return mOrientation;
			}
			void Transform::setPosition(const Vector3 & position)
			{
				mPosition = position;
			}
			void Transform::setScale(const Vector3 & scale)
			{
				mScale = scale;
			}
			void Transform::setOrientation(const Quaternion & orientation)
			{
				mOrientation = orientation;
			}
			void Transform::translate(const Vector3 & v)
			{
				mPosition += v;
			}
			void Transform::rotate(const Quaternion & q)
			{
				mOrientation *= q;
			}
		}
	}
}
