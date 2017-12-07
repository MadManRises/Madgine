#include "ogrelib.h"

#include "ogreanimationsystem.h"

#include "scene/entity/components/ogreanimation.h"

namespace Engine
{
	API_IMPL(Scene::Entity::OgreAnimationSystem);

	template <> thread_local Scene::Entity::OgreAnimationSystem *Singleton<Scene::Entity::OgreAnimationSystem>::sSingleton = nullptr;

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
