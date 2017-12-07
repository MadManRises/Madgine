#pragma once

#include "scene/entity/system.h"


namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class OGREMADGINE_EXPORT OgreAnimationSystem : public System<OgreAnimationSystem, Animation>
			{
			public:
				using System::System;

				void update(float timeSinceLastFrame) override;
			};
		}
	}
}
