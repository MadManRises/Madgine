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
        mProgram = ProgramLoader::getSingleton().getOrCreateManual("scene", {}, [](const ProgramLoader::ResourceType *res) {
            return ProgramLoader::getSingleton().create("scene");
        });

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "tex", 0);
    }

    void Im3DRenderPass::render(RenderTarget *target)
    {
        Im3D::Im3DContext *context = Im3D::GetCurrentContext();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "v", mCamera->getViewMatrix());
        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "p", mCamera->getProjectionMatrix(aspectRatio));

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasLight", false);

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasDistanceField", false);

        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "m", Matrix4::IDENTITY);
        ProgramLoader::getSingleton().setUniform(*mProgram->loadData(),
            "anti_m",
            Matrix3::IDENTITY);

        target->clearDepthBuffer();

        /*for (const std::pair<Im3DNativeMesh, std::vector<Matrix4>> &p : context->mNativeMeshes)
            target->renderInstancedMesh(RenderPassFlags_NoLighting, p.first, p.second);*/

        for (std::pair<const Im3DTextureId, Im3D::Im3DContext::RenderData> &p : context->mRenderData) {

            ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasTexture", p.first != 0);
            TextureLoader::getSingleton().bind(p.first);

            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                target->renderVertices(p.second.mFlags, i + 1, p.second.mVertices[i].data(), p.second.mVertices[i].size(), p.second.mIndices[i].data(), p.second.mIndices[i].size());
                target->renderVertices(p.second.mFlags, i + 1, p.second.mVertices2[i].data(), p.second.mVertices2[i].size(), p.second.mIndices2[i].data(), p.second.mIndices2[i].size(), p.first);
            }
        }
    }

    int Im3DRenderPass::priority() const
    {
        return mPriority;
    }

}
}
