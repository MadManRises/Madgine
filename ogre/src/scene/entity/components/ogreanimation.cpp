#include "ogrelib.h"

#include "ogremesh.h"

#include "ogreanimation.h"

namespace Engine
{
	API_IMPL(Scene::Entity::Animation);

	namespace Scene
	{
		namespace Entity
		{
			template <>
			const char* const EntityComponent<Animation>::sComponentName = "Animation";

			Animation::Animation(Entity& entity, const Scripting::LuaTable& table) :
				SystemComponent(entity, table),
				mMesh(nullptr),
				mState(nullptr),
				mDefaultState(nullptr),
				mDefaultAnimation(table.getValue("default").asDefault<std::string>(""))
			{
			}


			Animation::~Animation()
			{
			}

			void Animation::init()
			{
				mMesh = getEntity().getComponent<Mesh>();
				if (!mDefaultAnimation->empty())
				{
					setDefaultAnimation(mDefaultAnimation);
				}
				SystemComponent::init();
			}

			void Animation::finalize()
			{
				SystemComponent::finalize();
			}

			void Animation::update(float timestep)
			{
				if (mState)
				{
					mState->addTime(timestep);
					if (mState->hasEnded())
					{
						resetAnimation();
					}
				}
			}

			void Animation::resetAnimation()
			{
				if (mState)
				{
					mState->setEnabled(false);
				}
				mState = mDefaultState;
				if (mState)
				{
					mState->setEnabled(true);
					mState->setTimePosition(0.0f);
				}
			}

			void Animation::setAnimation(const std::string& name, LoopSetting loop)
			{
				if (mState)
				{
					mState->setEnabled(false);
				}
				mState = mMesh->getMesh()->getAnimationState(name);
				if (mState)
				{
					switch (loop)
					{
					case DEFAULT:
						break;
					case LOOP:
						mState->setLoop(true);
						break;
					case NO_LOOP:
						mState->setLoop(false);
						break;
					}
					mState->setTimePosition(0.0f);
					mState->setEnabled(true);
				}
			}

			void Animation::setDefaultAnimation(const std::string& name)
			{
				mDefaultAnimation = name;
				if (mMesh)
				{
					mDefaultState = mMesh->getMesh()->getAnimationState(name);
					if (mDefaultState)
					{
						mDefaultState->setLoop(true);
						if (!mState)
						{
							resetAnimation();
						}
					}
				}
			}

			void Animation::resetDefaultAnimation()
			{
				mDefaultAnimation->clear();
				if (mDefaultState)
				{
					mDefaultState = nullptr;
				}
			}

			float Animation::getAnimationLength(const std::string& name) const
			{
				if (mMesh)
				{
					Ogre::AnimationState* state = mMesh->getMesh()->getAnimationState(name);
					if (state)
					{
						return state->getLength();
					}
				}
				return 0.0f;
			}
		}
	}
}
