#include "../clienttoolslib.h"

#include "im3drenderpass.h"

#include "Madgine/render/rendertarget.h"

#include "im3d/im3d.h"
#include "im3d/im3d_internal.h"

#include "Madgine/render/camera.h"

#include "textureloader.h"

namespace Engine {
namespace Render {

    Im3DRenderPass::Im3DRenderPass(Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {
        mProgram.create("scene");
        
        mPerObject.hasLight = false;
        mPerObject.hasDistanceField = false;

        mPerObject.m = Matrix4::IDENTITY;
        mPerObject.anti_m = Matrix4::IDENTITY;
    }

    void Im3DRenderPass::render(RenderTarget *target)
    {
        Im3D::Im3DContext *context = Im3D::GetCurrentContext();

        target->clearDepthBuffer();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        mPerApplication.p = mCamera->getProjectionMatrix(aspectRatio);

        mProgram.setParameters(mPerApplication, 0);

        mPerFrame.v = mCamera->getViewMatrix();

        mProgram.setParameters(mPerFrame, 1);

        /*for (const std::pair<Im3DNativeMesh, std::vector<Matrix4>> &p : context->mNativeMeshes)
            target->renderInstancedMesh(RenderPassFlags_NoLighting, p.first, p.second);*/

        for (std::pair<const Im3DTextureId, Im3D::Im3DContext::RenderData> &p : context->mRenderData) {

            mPerObject.hasTexture = p.first != 0;
            mProgram.setParameters(mPerObject, 2);
            TextureLoader::getSingleton().bind(p.first);

            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                target->renderVertices(mProgram, p.second.mFlags, i + 1, p.second.mVertices[i], p.second.mIndices[i]);
                target->renderVertices(mProgram, p.second.mFlags, i + 1, p.second.mVertices2[i], p.second.mIndices2[i], p.first);
            }
        }
    }

    int Im3DRenderPass::priority() const
    {
        return mPriority;
    }

}
}
