#include "../../../serverlib.h"

#include "servertransform.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(ServerTransform, Transform);

			ServerTransform::ServerTransform(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponentVirtualImpl(entity, table)
			{
			}

			ServerTransform::~ServerTransform()
			{
			}


			void ServerTransform::setPosition(const Vector3& v)
			{
				mPosition = v;
			}

			void ServerTransform::setScale(const Vector3& scale)
			{
				mScale = scale;
			}

			void ServerTransform::setOrientation(const std::array<float, 4>& orientation)
			{
				mOrientation = orientation;
			}

			void ServerTransform::translate(const Vector3& v)
			{
				//mNode->translate(Ogre::Vector3(v.ptr()));
			}

			void ServerTransform::rotate(const std::array<float, 4>& q)
			{
				//mNode->rotate(Ogre::Quaternion(const_cast<float*>(q.data())));
			}

			Vector3 ServerTransform::getPosition() const
			{
				return mPosition;
			}

			std::array<float, 4> ServerTransform::getOrientation() const
			{
				return mOrientation;
			}

			Vector3 ServerTransform::getScale() const
			{
				return mScale;
			}
		}
	}
}
