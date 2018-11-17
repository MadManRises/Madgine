#include "../../ogrelib.h"

#include "ogretransform.h"

#include "../ogrescenerenderer.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(OgreTransform, Transform);

			Entity* OgreTransform::entityFromNode(Ogre::SceneNode* node)
			{
				const Ogre::Any& any =
					node->getUserObjectBindings().getUserAny("entity");

				return any.isEmpty() ? nullptr : Ogre::any_cast<Entity *>(any);
			}

			OgreTransform::OgreTransform(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponentVirtualImpl(entity, table),
				mNode(nullptr)
			{
				mNode = getGlobalAPIComponent<OgreSceneRenderer>().createEntityNode();
				mNode->getUserObjectBindings().setUserAny("entity", Ogre::Any(&getEntity()));
			}

			OgreTransform::~OgreTransform()
			{
				if (mNode->getAttachedObjectIterator().hasMoreElements())
				LOG_ERROR(Database::Exceptions::nodeNotCleared);

				mNode->getParentSceneNode()->removeChild(mNode);
				mNode->getCreator()->destroySceneNode(mNode);
			}

			void OgreTransform::init()
			{
			}

			void OgreTransform::finalize()
			{
			}


			Ogre::SceneNode* OgreTransform::getNode() const
			{
				return mNode;
			}


			void OgreTransform::setPosition(const Vector3& v)
			{
				mNode->setPosition(Ogre::Vector3(v.ptr()));
			}

			void OgreTransform::setScale(const Vector3& scale)
			{
				mNode->setScale(Ogre::Vector3(scale.ptr()));
			}

			void OgreTransform::setOrientation(const std::array<float, 4>& orientation)
			{
				mNode->setOrientation(Ogre::Quaternion(const_cast<float*>(orientation.data())));
			}

			void OgreTransform::translate(const Vector3& v)
			{
				mNode->translate(Ogre::Vector3(v.ptr()));
			}

			void OgreTransform::rotate(const std::array<float, 4>& q)
			{
				mNode->rotate(Ogre::Quaternion(const_cast<float*>(q.data())));
			}

			void OgreTransform::lookAt(const Vector3& v)
			{
				mNode->lookAt(Ogre::Vector3(v.ptr()), Ogre::Node::TS_WORLD);
			}

			Vector3 OgreTransform::getPosition() const
			{
				return {mNode->getPosition().x, mNode->getPosition().y, mNode->getPosition().z};
			}

			std::array<float, 4> OgreTransform::getOrientation() const
			{
				return {
					{mNode->getOrientation().w, mNode->getOrientation().x, mNode->getOrientation().y, mNode->getOrientation().z}
				};
			}

			Vector3 OgreTransform::getScale() const
			{
				return {mNode->getScale().x, mNode->getScale().y, mNode->getScale().z};
			}
		}
	}
}
