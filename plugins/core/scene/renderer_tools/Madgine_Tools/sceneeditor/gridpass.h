#pragma once

#include "Madgine/render/renderpass.h"
/*#include "OpenGL/util/openglprogram.h"
#include "OpenGL/util/openglbuffer.h"
#include "OpenGL/util/openglvertexarray.h"*/
#include "Madgine/meshloader/gpumeshloader.h"
#include "Madgine/pipelineloader/pipelineloader.h"

namespace Engine {
namespace Tools {

    struct GridPass : Render::RenderPass {

        GridPass(Render::Camera *camera, int priority);

        virtual void render(Render::RenderTarget *target, size_t iteration) override;

        virtual int priority() const override;

    private:
        Render::GPUMeshLoader::Handle mMesh;
        Render::PipelineLoader::Instance mPipeline;

        Render::Camera *mCamera;

        int mPriority;
    };

}
}