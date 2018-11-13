#pragma once

#include "../entitycomponent.h"
#include "Interfaces/serialize/container/serializedmapper.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{

			class MADGINE_BASE_EXPORT Mesh : public EntityComponentVirtualBase<Mesh>
			{
			public:
				using EntityComponentVirtualBase<Mesh>::EntityComponentVirtualBase;
				virtual ~Mesh() = default;

				virtual std::string getName() const = 0;
				virtual void setName(const std::string &name) = 0;

				virtual void setVisible(bool vis) = 0;
				virtual bool isVisible() const = 0;

				KeyValueMapList maps() override;

			private:
				std::string getName2() const
				{
					return getName();
				}

				void setName2(const std::string &name)
				{
					setName(name);
				}

				Serialize::SerializedMapper<&Mesh::getName2, &Mesh::setName2>
					mSerializedObject;

			};

		}
	}
}

RegisterClass(Engine::Scene::Entity::Mesh);