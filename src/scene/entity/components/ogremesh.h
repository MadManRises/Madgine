#pragma once

#include "../entitycomponent.h"
#include "../../../serialize/container/serializedmapper.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class OGREMADGINE_EXPORT Mesh : public EntityComponent<Mesh>
			{
			public:
				Mesh(Entity& entity, const Scripting::LuaTable& table);
				Mesh(Entity& entity, const std::string& meshName = "");
				virtual ~Mesh();

				void init() override;
				void finalize() override;


				Ogre::Entity* getMesh() const;

				Vector3 getCenter() const;

				std::string getName() const;
				void setName(const std::string& mesh);

				bool isVisible() const;
				void setVisible(bool b);

				void destroyObject();

			private:
				Transform* mTransform;
				Ogre::Entity* mObject;
				Serialize::SerializedMapper<decltype(&Mesh::getName), &Mesh::getName, decltype(&Mesh::setName), &Mesh::setName>
				mSerializedObject;
			};
		}
	}
}
