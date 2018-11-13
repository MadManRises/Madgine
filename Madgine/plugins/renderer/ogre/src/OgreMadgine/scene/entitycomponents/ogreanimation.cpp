#include "../../ogrelib.h"

#include "ogremesh.h"

#include "ogreanimation.h"

#include "Interfaces/generic/valuetype.h"

#include "../ogreanimationsystem.h"

namespace Engine
{

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENTVIRTUALIMPL_IMPL(OgreAnimation, Animation);

			OgreAnimation::OgreAnimation(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponentVirtualImpl(entity, table),
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
				mMesh = getComponent<OgreMesh>();
				if (!mDefaultAnimation.empty())
				{
					setDefaultAnimation(mDefaultAnimation);
				}
				getSceneComponent<OgreAnimationSystem>().addComponent(this);
				EntityComponentVirtualImpl::init();
			}

			void OgreAnimation::finalize()
			{
				getSceneComponent<OgreAnimationSystem>().removeComponent(this);
				EntityComponentVirtualImpl::finalize();
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

