#include "../../../ogrelib.h"

#include "ogremesh.h"

#include "ogreanimation.h"

namespace Engine
{
	API_IMPL(Scene::Entity::Animation);

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(OgreAnimation, Animation);

			OgreAnimation::OgreAnimation(Entity& entity, const Scripting::LuaTable& table) :
				SystemComponent(entity, table),
				mMesh(nullptr),
				mState(nullptr),
				mDefaultState(nullptr),
				mDefaultAnimation(table["default"].asDefault<std::string>(""))
			{
			}


			OgreAnimation::~OgreAnimation()
			{
			}

			void OgreAnimation::init()
			{
				mMesh = getEntity().getComponent<OgreMesh>();
				if (!mDefaultAnimation.empty())
				{
					setDefaultAnimation(mDefaultAnimation);
				}
				SystemComponent::init();
			}

			void OgreAnimation::finalize()
			{
				SystemComponent::finalize();
			}

			void OgreAnimation::update(float timestep)
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

			void OgreAnimation::resetAnimation()
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

			std::string OgreAnimation::getDefaultAnimation() const
			{
				return mDefaultAnimation;
			}

			void OgreAnimation::setAnimation(const std::string& name, LoopSetting loop)
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

			void OgreAnimation::setDefaultAnimation(const std::string& name)
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

			void OgreAnimation::resetDefaultAnimation()
			{
				mDefaultAnimation.clear();
				if (mDefaultState)
				{
					mDefaultState = nullptr;
				}
			}

			float OgreAnimation::getAnimationLength(const std::string& name) const
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
