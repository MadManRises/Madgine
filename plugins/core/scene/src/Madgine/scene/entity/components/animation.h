#pragma once

#include "../entitycomponent.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{

			class MADGINE_SCENE_EXPORT Animation : public EntityComponent<Animation>
			{
			public:
				using EntityComponent<Animation>::EntityComponent;
				virtual ~Animation() = default;

				virtual std::string getDefaultAnimation() const = 0;
				void resetDefaultAnimation();
				virtual void setDefaultAnimation(const std::string &name) = 0;

			};

		}
	}
}
