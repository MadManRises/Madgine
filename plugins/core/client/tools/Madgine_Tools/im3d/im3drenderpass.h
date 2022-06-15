#pragma once

#include "Madgine/render/renderpass.h"

#include "pipelineloader.h"

#include "gpumeshloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_TOOLS_EXPORT Im3DRenderPass : RenderPass {
        Im3DRenderPass(Camera *camera, int priority);

        virtual void render(RenderTarget *target, size_t iteration) override;

        virtual int priority() const override;

    private:
        PipelineLoader::Instance mPipeline;

        GPUMeshLoader::PtrType mMeshes[/* IM3D_MESHTYPE_COUNT*/ 3][2];

        Camera *mCamera;

        int mPriority;
    };

}
}