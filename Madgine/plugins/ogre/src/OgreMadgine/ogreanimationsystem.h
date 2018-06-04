#pragma once

#include "Madgine/scene/entity/system.h"


namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class OGREMADGINE_EXPORT OgreAnimationSystem : public System<OgreAnimationSystem, OgreAnimation>
			{
			public:
				using System::System;

				void update(float timeSinceLastFrame) override;
			};
		}
	}
}
