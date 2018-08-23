#include "../ogrelib.h"

#include "ogreanimationsystem.h"

#include "entitycomponents/ogreanimation.h"

RegisterClass(Engine::Scene::Entity::OgreAnimationSystem);

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
