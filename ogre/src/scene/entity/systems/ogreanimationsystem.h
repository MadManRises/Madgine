#pragma once

#include "scene/entity/system.h"
#include "scene/entity/components/ogreanimation.h"
#include "scene/scenecomponent.h"

namespace Engine {
	namespace Scene {
		namespace Entity {

			class OGREMADGINE_EXPORT OgreAnimationSystem : public System<Animation>, public SceneComponent<OgreAnimationSystem> {
			public:
				using SerializableUnit::SerializableUnit;
				
				virtual void update(float timeSinceLastFrame) override;
			};

		}
	}
}