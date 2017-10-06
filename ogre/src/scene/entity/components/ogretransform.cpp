#include "ogrelib.h"

#include "ogretransform.h"

#include "Scene/ogrescenemanager.h"

namespace Engine {

	API_IMPL(Scene::Entity::Transform, MAP_RO(Position, getPosition), MAP_RO(Orientation, getOrientation), MAP_RO(Scale, getScale));

	namespace Scene {
		namespace Entity {

			template <>
			const char * const EntityComponent<Transform>::sComponentName = "Transform";

			Entity *Transform::entityFromNode(Ogre::SceneNode *node)
			{
				const Ogre::Any &any =
					node->getUserObjectBindings().getUserAny("entity");

				return any.isEmpty() ? 0 : Ogre::any_cast<Entity *>(any);
			}

			Transform::Transform(Entity &entity, const Scripting::LuaTable &table) :
				EntityComponent(entity, table),
				mNode(nullptr)
			{	
				mNode = static_cast<OgreSceneManager&>(getEntity().sceneMgr()).createEntityNode();
				mNode->getUserObjectBindings().setUserAny("entity", Ogre::Any(&getEntity()));
			}

			Transform::~Transform() {
			}

			void Transform::init()
			{
				
			}

			void Transform::finalize()
			{
				if (mNode) {
					if (mNode->getAttachedObjectIterator().hasMoreElements())
						LOG_ERROR(Exceptions::nodeNotCleared);

					mNode->getParentSceneNode()->removeChild(mNode);
					mNode->getCreator()->destroySceneNode(mNode);
					mNode = nullptr;
				}
			}

			

			Ogre::SceneNode *Transform::getNode() const
			{
				return mNode;
			}


			void Transform::setPosition(const Vector3 &v)
			{
				mNode->setPosition(Ogre::Vector3(v.ptr()));
			}

			void Transform::setScale(const Vector3 & scale)
			{
				mNode->setScale(Ogre::Vector3(scale.ptr()));
			}

			void Transform::setOrientation(const std::array<float, 4>& orientation)
			{
				mNode->setOrientation(Ogre::Quaternion(const_cast<float*>(orientation.data())));
			}

			void Transform::translate(const Vector3 & v)
			{
				mNode->translate(Ogre::Vector3(v.ptr()));
			}

			void Transform::rotate(const std::array<float, 4> &q)
			{
				mNode->rotate(Ogre::Quaternion(const_cast<float*>(q.data())));
			}

			Vector3 Transform::getPosition() const
			{
				return{ mNode->getPosition().x, mNode->getPosition().y, mNode->getPosition().z };
			}

			std::array<float, 4> Transform::getOrientation() const
			{
				return{ {mNode->getOrientation().w, mNode->getOrientation().x, mNode->getOrientation().y, mNode->getOrientation().z} };
			}

			Vector3 Transform::getScale() const
			{
				return{ mNode->getScale().x, mNode->getScale().y, mNode->getScale().z };
			}


		}
	}


}
