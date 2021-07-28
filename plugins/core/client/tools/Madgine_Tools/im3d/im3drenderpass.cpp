#include "../clienttoolslib.h"

#include "im3drenderpass.h"

#include "Madgine/render/rendertarget.h"

#include "im3d/im3d.h"
#include "im3d/im3d_internal.h"

#include "Madgine/render/camera.h"

#include "textureloader.h"

#include "Madgine/render/shadinglanguage/sl.h"

#define SL_SHADER im3d
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Render {

    Im3DRenderPass::Im3DRenderPass(Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {
        mProgram.create("im3d");
        
        mProgram.setParameters(0, sizeof(Im3DPerApplication));
        mProgram.setParameters(1, sizeof(Im3DPerFrame));
        mProgram.setParameters(2, sizeof(Im3DPerObject));
    }

    void Im3DRenderPass::render(RenderTarget *target)
    {
        Im3D::Im3DContext *context = Im3D::GetCurrentContext();

        target->clearDepthBuffer();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        {
            auto perApplication = mProgram.mapParameters(0).cast<Im3DPerApplication>();

            perApplication->p = mCamera->getProjectionMatrix(aspectRatio);
        }

        {
            auto perFrame = mProgram.mapParameters(1).cast<Im3DPerFrame>();

            perFrame->v = mCamera->getViewMatrix();
        }

        /*for (const std::pair<Im3DNativeMesh, std::vector<Matrix4>> &p : context->mNativeMeshes)
            target->renderInstancedMesh(RenderPassFlags_NoLighting, p.first, p.second);*/

        for (std::pair<const Im3DTextureId, Im3D::Im3DContext::RenderData> &p : context->mRenderData) {

            target->bindTexture({ p.first });

            {
                auto perObject = mProgram.mapParameters(2).cast<Im3DPerObject>();

                perObject->hasDistanceField = false;

                perObject->m = Matrix4::IDENTITY;

                perObject->hasTexture = p.first != 0;
                perObject->hasDistanceField = bool(p.second.mFlags & RenderPassFlags_DistanceField);
            }           

            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                target->renderVertices(mProgram, i + 1, p.second.mVertices[i], p.second.mIndices[i]);
                target->renderVertices(mProgram, i + 1, p.second.mVertices2[i], p.second.mIndices2[i], p.first);
            }
        }
    }

    int Im3DRenderPass::priority() const
    {
        return mPriority;
    }

}
}
