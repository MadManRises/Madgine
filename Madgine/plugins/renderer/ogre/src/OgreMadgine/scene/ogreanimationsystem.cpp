#include "../ogrelib.h"

#include "ogreanimationsystem.h"

#include "entitycomponents/ogreanimation.h"

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			void OgreAnimationSystem::update(float timeSinceLastFrame)
			{
				for (OgreAnimation* anim : components())
				{
					anim->update(timeSinceLastFrame);
				}
			}
		}
	}
}

