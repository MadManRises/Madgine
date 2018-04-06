#include "../../../serverlib.h"

#include "servertransform.h"

namespace Engine
{
	API_IMPL(Scene::Entity::Transform, MAP_RO(Position, getPosition), MAP_RO(Orientation, getOrientation), MAP_RO(Scale,
		getScale));

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENT_IMPL(Transform, Transform);

			Transform::Transform(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponent(entity, table)
			{
			}

			Transform::~Transform()
			{
			}


			void Transform::setPosition(const Vector3& v)
			{
				mPosition = v;
			}

			void Transform::setScale(const Vector3& scale)
			{
				mScale = scale;
			}

			void Transform::setOrientation(const std::array<float, 4>& orientation)
			{
				mOrientation = orientation;
			}

			void Transform::translate(const Vector3& v)
			{
				//mNode->translate(Ogre::Vector3(v.ptr()));
			}

			void Transform::rotate(const std::array<float, 4>& q)
			{
				//mNode->rotate(Ogre::Quaternion(const_cast<float*>(q.data())));
			}

			Vector3 Transform::getPosition() const
			{
				return mPosition;
			}

			std::array<float, 4> Transform::getOrientation() const
			{
				return mOrientation;
			}

			Vector3 Transform::getScale() const
			{
				return mScale;
			}
		}
	}
}
