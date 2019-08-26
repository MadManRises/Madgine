#include "../../../serverlib.h"

#include "servermesh.h"

#include "Modules/keyvalue/valuetype.h"

#include "Madgine/scene/entity/components/transform.h"
#include "Modules/serialize/serializetable_impl.h"

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(ServerMesh);

			ServerMesh::ServerMesh(Entity &entity, const ObjectPtr &table)
                            :
				EntityComponentVirtualImpl<Engine::Scene::Entity::ServerMesh, Engine::Scene::Entity::Mesh>(entity, table),
				mTransform(nullptr),
				mMeshName(table["mesh"].as<std::string>())
			{
			}

			ServerMesh::ServerMesh(Entity& entity, const std::string& meshName) :
				EntityComponentVirtualImpl<Engine::Scene::Entity::ServerMesh, Engine::Scene::Entity::Mesh>(entity),
				mTransform(nullptr),
				mMeshName(meshName)
			{
			}

			ServerMesh::~ServerMesh()
			{
			}

			void ServerMesh::init()
			{
				mTransform = getComponent<Transform>();
			}

			std::string ServerMesh::getName() const
			{
				return mMeshName;
			}

			void ServerMesh::setVisible(bool b)
			{
			}

			bool ServerMesh::isVisible() const
			{
				return true;
			}

			Vector3 ServerMesh::getCenter() const
			{
				return mTransform->getPosition();
			}

			void ServerMesh::setName(const std::string& name)
			{
				mMeshName = name;
			}
		}
	}
}

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::ServerMesh)
SERIALIZETABLE_END(Engine::Scene::Entity::ServerMesh)