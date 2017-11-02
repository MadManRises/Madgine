#include "ogrelib.h"

#include "ogremesh.h"

#include "ogretransform.h"

#include "Scene/ogrescenemanager.h"

namespace Engine {

	API_IMPL(Scene::Entity::Mesh, MAP_RO(MeshName, getName), MAP_RO(Visible, isVisible));

	namespace Scene {
		namespace Entity {

			template <>
			const char * const EntityComponent<Mesh>::sComponentName = "Mesh";

			Mesh::Mesh(Entity &entity, const Scripting::LuaTable &table) :
				Mesh(entity, table.getValue("mesh").asDefault<std::string>(""))
			{}

			Mesh::Mesh(Entity & entity, const std::string & meshName) :
				EntityComponent(entity),
				mObject(nullptr),
				mTransform(nullptr)
			{
				setName(meshName);
			}


			Mesh::~Mesh() {
				assert(!mObject);
			}

			void Mesh::init()
			{
				mTransform = getEntity().addComponent_t<Transform>();
				if (mObject)
					mTransform->getNode()->attachObject(mObject);
			}

			void Mesh::finalize()
			{
				destroyObject();
				
			}

			std::string Mesh::getName() const
			{
				return mObject ? mObject->getMesh()->getName() : "";
			}

			void Mesh::setName(const std::string & mesh)
			{
				destroyObject();
				if (!mesh.empty()) {
					mObject = static_cast<OgreSceneManager&>(getEntity().sceneMgr()).getSceneManager()->createEntity(mesh);
					mObject->addQueryFlags(Masks::ENTITY_MASK);
					if (mTransform)
						mTransform->getNode()->attachObject(mObject);
				}
			}

			bool Mesh::isVisible() const
			{
				return mObject ? mObject->isVisible() : false;
			}

			void Mesh::setVisible(bool b)
			{
				if (mObject)
					mObject->setVisible(b);
			}

			Ogre::Entity *Mesh::getMesh()
			{
				return mObject;
			}

			void Mesh::destroyObject() {
				if (mObject) {
					if (mTransform)
						mTransform->getNode()->detachObject(mObject);
					static_cast<OgreSceneManager&>(getEntity().sceneMgr()).getSceneManager()->destroyEntity(mObject);
					mObject = nullptr;
				}
			}

			Vector3 Mesh::getCenter() const
			{
				if (mObject) {
					Ogre::Vector3 v = mObject->getWorldBoundingBox().getCenter();
					return{ v.x, v.y, v.z };
				}
				else {
					return mTransform->getPosition();
				}	
			}


		}
	}
}