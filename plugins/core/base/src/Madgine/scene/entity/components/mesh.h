#pragma once

#include "../entitycomponent.h"

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

				virtual AABB aabb() const = 0;

				virtual void setVisible(bool vis) = 0;
				virtual bool isVisible() const = 0;

			};

		}
	}
}