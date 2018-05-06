#pragma once

#include "../system.h"


namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_CLIENT_EXPORT OgreAnimationSystem : public System<OgreAnimationSystem, OgreAnimation>
			{
			public:
				using System::System;

				void update(float timeSinceLastFrame) override;
			};
		}
	}
}
