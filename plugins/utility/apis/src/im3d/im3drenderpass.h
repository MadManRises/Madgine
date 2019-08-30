#pragma once

#include "Madgine/render/renderpass.h"


namespace Engine {
namespace Render {

    struct MADGINE_APIS_EXPORT Im3DRenderPass : RenderPass {

        virtual void render(RenderTarget *target, Scene::Camera *camera) override;
    };

}
}