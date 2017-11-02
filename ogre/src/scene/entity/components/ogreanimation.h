#pragma once


#include "Scene/Entity/systemcomponent.h"

#include "serialize/container/serialized.h"

namespace Engine {
	namespace Scene {
		namespace Entity {

			class OGREMADGINE_EXPORT Animation : public SystemComponent<Animation> {
			public:
				enum LoopSetting {
					DEFAULT,
					LOOP,
					NO_LOOP
				};

				Animation(Entity &entity, const Scripting::LuaTable &table);
				virtual ~Animation();

				virtual void init() override;
				virtual void finalize() override;

				void update(float timestep);

				void setAnimation(const std::string &name, LoopSetting loop = DEFAULT);
				void resetAnimation();
				void setDefaultAnimation(const std::string &name);
				void resetDefaultAnimation();

			private:
				Mesh *mMesh;
				Ogre::AnimationState *mState, *mDefaultState;
				Serialize::Serialized<std::string> mDefaultAnimation;
			};
		}
	}
}