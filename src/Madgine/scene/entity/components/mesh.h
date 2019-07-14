#pragma once

#include "../entitycomponent.h"
#include "Modules/serialize/container/serializedmapper.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{

			class MADGINE_BASE_EXPORT Mesh : public EntityComponent<Mesh>
			{
			public:
				using EntityComponent<Mesh>::EntityComponent;
				virtual ~Mesh() = default;

				virtual std::string getName() const = 0;
				virtual void setName(const std::string &name) = 0;

				virtual void setVisible(bool vis) = 0;
				virtual bool isVisible() const = 0;

				//KeyValueMapList maps() override;

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