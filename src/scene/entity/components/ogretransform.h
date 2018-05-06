#pragma once

#include "transform.h"

#include "../../../serialize/container/serializedmapper.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_CLIENT_EXPORT OgreTransform : public EntityComponentVirtualImpl<OgreTransform, Transform>
			{
			public:
				static Entity* entityFromNode(Ogre::SceneNode* node);

				OgreTransform(Entity& entity, const Scripting::LuaTable& table = {});
				virtual ~OgreTransform();

				void init() override;
				void finalize() override;

				Ogre::SceneNode* getNode() const;

				Vector3 getPosition() const;
				Vector3 getScale() const;
				std::array<float, 4> getOrientation() const;

				void setPosition(const Vector3& v);
				void setScale(const Vector3& scale);
				void setOrientation(const std::array<float, 4>& orientation);

				void translate(const Vector3& v);
				void rotate(const std::array<float, 4>& q);

				void lookAt(const Vector3& v);

			private:
				Ogre::SceneNode* mNode;
				Serialize::SerializedMapper<decltype(&Transform::getPosition), &Transform::getPosition, decltype(&Transform::
					                            setPosition), &Transform::setPosition> mSerializedPosition;
			};
		}
	}
}
