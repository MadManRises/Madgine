#include "../clienttoolslib.h"

#include "im3drenderpass.h"

#include "Madgine/render/rendertarget.h"

#include "im3d/im3d.h"
#include "im3d/im3d_internal.h"

#include "Madgine/render/camera.h"

namespace Engine {
namespace Render {

    Im3DRenderPass::Im3DRenderPass(Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {
        mProgram.create("scene");

        mProgram.setUniform("tex", 0);
    }

    void Im3DRenderPass::render(RenderTarget *target)
    {
        Im3D::Im3DContext *context = Im3D::GetCurrentContext();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        mProgram.setUniform("v", mCamera->getViewMatrix());
        mProgram.setUniform("p", mCamera->getProjectionMatrix(aspectRatio));

        mProgram.setUniform("hasLight", false);

        mProgram.setUniform("hasDistanceField", false);

        mProgram.setUniform("m", Matrix4::IDENTITY);
        mProgram.setUniform(
            "anti_m",
            Matrix3::IDENTITY);

        target->clearDepthBuffer();

        /*for (const std::pair<Im3DNativeMesh, std::vector<Matrix4>> &p : context->mNativeMeshes)
            target->renderInstancedMesh(RenderPassFlags_NoLighting, p.first, p.second);*/

        for (std::pair<const Im3DTextureId, Im3D::Im3DContext::RenderData> &p : context->mRenderData) {

			mProgram.setUniform("hasTexture", p.first != 0);
            TextureLoader::getSingleton().bind(p.first);

            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                target->renderVertices(p.second.mFlags, i + 1, p.second.mVertices[i], p.second.mIndices[i]);
                target->renderVertices(p.second.mFlags, i + 1, p.second.mVertices2[i], p.second.mIndices2[i], p.first);
            }
        }
    }

    int Im3DRenderPass::priority() const
    {
        return mPriority;
    }

}
}
