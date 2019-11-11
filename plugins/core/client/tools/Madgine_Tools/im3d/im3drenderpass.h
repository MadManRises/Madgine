#pragma once

#include "Madgine/render/renderpass.h"

#include "programloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_TOOLS_EXPORT Im3DRenderPass : RenderPass {
        Im3DRenderPass(Camera *camera);

        virtual void render(RenderTarget *target) override;

    private:
        ProgramLoader::ResourceType *mProgram;

        Camera *mCamera;
    };

}
}