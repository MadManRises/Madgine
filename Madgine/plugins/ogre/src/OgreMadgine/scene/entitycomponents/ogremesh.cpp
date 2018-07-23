#include "../../ogrelib.h"

#include "ogremesh.h"

#include "ogretransform.h"

#include "../ogrescenerenderer.h"

#include "Madgine/scene/entity/masks.h"

#include "Madgine/app/application.h"
#include "../../resources/ogremeshloader.h"

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(OgreMesh, Mesh);

			OgreMesh::OgreMesh(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponentVirtualImpl<Engine::Scene::Entity::OgreMesh, Engine::Scene::Entity::Mesh>(entity, table),
			mTransform(nullptr),
			mObject(nullptr)
			{
				setName(table["mesh"].asDefault<std::string>(""));
			}


			OgreMesh::~OgreMesh()
			{
				assert(!mObject);
			}

			void OgreMesh::init()
			{
				mTransform = getEntity().addComponent_t<OgreTransform>();
				if (mObject)
					mTransform->getNode()->attachObject(mObject);
			}

			void OgreMesh::finalize()
			{
				destroyObject();
			}

			std::string OgreMesh::getName() const
			{
				return mObject ? mObject->getMesh()->getName() : "";
			}

			void OgreMesh::setName(const std::string& mesh)
			{
				destroyObject();
				if (!mesh.empty())
				{
					auto p = app().resources().load<OgreMeshLoader>(mesh);
					mObject = getGlobalAPIComponent<OgreSceneRenderer>().getSceneManager()->createEntity(mesh);
					mObject->addQueryFlags(Masks::ENTITY_MASK);
					if (mTransform)
						mTransform->getNode()->attachObject(mObject);
				}
			}

			bool OgreMesh::isVisible() const
			{
				return mObject ? mObject->isVisible() : false;
			}

			void OgreMesh::setVisible(bool b)
			{
				if (mObject)
					mObject->setVisible(b);
			}

			Ogre::Entity* OgreMesh::getMesh() const
			{
				return mObject;
			}

			void OgreMesh::destroyObject()
			{
				if (mObject)
				{
					if (mTransform)
						mTransform->getNode()->detachObject(mObject);
					getGlobalAPIComponent<OgreSceneRenderer>().getSceneManager()->destroyEntity(mObject);
					mObject = nullptr;
				}
			}

			Vector3 OgreMesh::getCenter() const
			{
				if (mObject)
				{
					Ogre::Vector3 v = mObject->getWorldBoundingBox().getCenter();
					return {v.x, v.y, v.z};
				}
				return mTransform->getPosition();
			}
		}
	}
}