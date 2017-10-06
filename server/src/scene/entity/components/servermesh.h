#pragma once

#include "Scene/Entity/entitycomponent.h"
#include "Serialize/container/serialized.h"

namespace Engine {
	namespace Scene {
		namespace Entity {

			class MADGINE_SERVER_EXPORT Mesh : public EntityComponent<Mesh> {

			public:
				Mesh(Entity &entity, const Scripting::LuaTable &table);
				Mesh(Entity &entity, const std::string &meshName);
				virtual ~Mesh();

				Vector3 getCenter() const;

				std::string getName() const;

				void setVisible(bool b);


			private:
				Transform *mTransform;
				Engine::Serialize::Serialized<std::string> mMeshName;				
			};
		}
	}
}