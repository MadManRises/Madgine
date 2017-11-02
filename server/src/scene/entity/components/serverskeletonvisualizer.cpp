#include "serverlib.h"

#include "servermesh.h"

#include "serverskeletonvisualizer.h"

namespace Engine {

	API_IMPL(Scene::Entity::ServerSkeletonVisualizer);

	namespace Scene {
		namespace Entity {

			template <>
			const char * const EntityComponent<ServerSkeletonVisualizer>::sComponentName = "SkeletonVisualizer";

			ServerSkeletonVisualizer::ServerSkeletonVisualizer(Entity &entity, const Scripting::LuaTable &table) :
				EntityComponent(entity, table)
			{}


			ServerSkeletonVisualizer::~ServerSkeletonVisualizer() {
			}


		}
	}
}