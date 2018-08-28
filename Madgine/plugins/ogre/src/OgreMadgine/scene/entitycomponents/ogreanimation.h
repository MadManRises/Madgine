#pragma once


#include "Madgine/scene/entity/components/animation.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class OgreAnimationSystem;

			class OGREMADGINE_EXPORT OgreAnimation : public EntityComponentVirtualImpl<OgreAnimation, Animation>
			{
			public:
				enum LoopSetting
				{
					DEFAULT,
					LOOP,
					NO_LOOP
				};

				OgreAnimation(Entity& entity, const Scripting::LuaTable& table);
				virtual ~OgreAnimation();

				void init() override;
				void finalize() override;

				void update(float timestep);

				void setAnimation(const std::string& name, LoopSetting loop = DEFAULT);
				void resetAnimation();
				std::string getDefaultAnimation() const override;
				void setDefaultAnimation(const std::string& name);
				void resetDefaultAnimation();
				float getAnimationLength(const std::string& name) const;

			private:
				OgreMesh* mMesh;
				Ogre::AnimationState *mState, *mDefaultState;
				std::string mDefaultAnimation;
			};
		}
	}
}
