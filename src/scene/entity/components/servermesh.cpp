#include "../../../serverlib.h"

#include "servermesh.h"

#include "servertransform.h"

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(ServerMesh, Mesh);

			ServerMesh::ServerMesh(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponentVirtualImpl<Engine::Scene::Entity::ServerMesh, Engine::Scene::Entity::Mesh>(entity, table),
				mTransform(nullptr),
				mMeshName(table.getValue("mesh").as<std::string>())
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
				mTransform = getEntity().getComponent<Transform>();
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
