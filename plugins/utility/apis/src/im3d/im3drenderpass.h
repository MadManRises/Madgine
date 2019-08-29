#pragma once

#include "Madgine/render/renderpass.h"


namespace Engine {
namespace Apis {

	struct MADGINE_APIS_EXPORT Im3DRenderPass : Render::RenderPass {

        virtual void render(Render::RenderTarget *target, Scene::Camera *camera) override;
    };

}
}