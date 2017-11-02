#include "serverlib.h"

#include "servermesh.h"

#include "servertransform.h"

namespace Engine {

	API_IMPL(Scene::Entity::Mesh);

	namespace Scene {
		namespace Entity {

			template <>
			const char * const EntityComponent<Mesh>::sComponentName = "Mesh";

			Mesh::Mesh(Entity &entity, const Scripting::LuaTable &table) :
				EntityComponent(entity, table),
				mTransform(nullptr),
				mMeshName(table.getValue("mesh").as<std::string>())
			{
				
			}

			Mesh::Mesh(Entity &entity, const std::string &meshName) :
				EntityComponent(entity),
				mTransform(nullptr),
				mMeshName(meshName)
			{
			}

			Mesh::~Mesh() {				
			}

			void Mesh::init()
			{
				mTransform = getEntity().getComponent<Transform>();
			}

			std::string Mesh::getName() const
			{
				return mMeshName;
			}

			void Mesh::setVisible(bool b)
			{
			}

			Vector3 Mesh::getCenter() const
			{
				return mTransform->getPosition();				
			}


		}
	}
}