#include "ogrelib.h"

#include "ogreanimationsystem.h"

namespace Engine
{
	API_IMPL(Scene::Entity::OgreAnimationSystem);

	namespace Scene
	{
		namespace Entity
		{
			void OgreAnimationSystem::update(float timeSinceLastFrame)
			{
				for (Animation* anim : components())
				{
					anim->update(timeSinceLastFrame);
				}
			}
		}
	}
}
