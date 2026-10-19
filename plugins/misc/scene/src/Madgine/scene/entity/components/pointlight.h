#pragma once

#include "Meta/math/color3.h"

#include "../entitycomponent.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        // Omnidirectional light at the position of the entity's Transform.
        // The radiance at distance d is mColor * mIntensity / (d^2 + 1), smoothly faded out to 0 at mRange.
        struct MADGINE_SCENE_EXPORT PointLight : EntityComponent<PointLight> {

            float mRange = 10.0f;
            Math::Color3 mColor = { 1.0f, 1.0f, 1.0f };
            float mIntensity = 1.0f;

            // Renders a shadow map for this light (6 faces), if the renderer has a free slot for it this frame. The forward
            // renderer (SceneRenderPass) only uses shadow casting point lights.
            bool mCastShadows = true;

            // Shadow casting and switched on: the lights the forward renderer (SceneRenderPass) uses
            bool castsShadow() const
            {
                return mCastShadows && mIntensity > 0.0f;
            }
        };

    }
}
}
