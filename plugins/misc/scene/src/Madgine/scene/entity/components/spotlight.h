#pragma once

#include "Meta/math/color3.h"

#include "../entitycomponent.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        // Cone shaped light at the position of the entity's Transform, shining along the local +Z axis of its orientation.
        // Falloff with distance like PointLight; across the cone the light fades from mInnerAngle to mOuterAngle (half
        // angles in degrees, measured from the axis).
        struct MADGINE_SCENE_EXPORT SpotLight : EntityComponent<SpotLight> {

            float mRange = 10.0f;
            Math::Color3 mColor = { 1.0f, 1.0f, 1.0f };
            float mIntensity = 1.0f;

            float mInnerAngle = 20.0f;
            float mOuterAngle = 30.0f;

            // Renders a shadow map for this light, if the renderer has a free slot for it this frame
            bool mCastShadows = false;
        };

    }
}
}
