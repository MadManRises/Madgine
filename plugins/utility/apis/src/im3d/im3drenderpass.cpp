#include "../apislib.h"

#include "im3drenderpass.h"

#include "Madgine/render/rendertarget.h"

#include "im3d.h"
#include "im3d_internal.h"

namespace Engine {
namespace Render {

    void Im3DRenderPass::render(RenderTarget *target, Scene::Camera *camera)
    {
        Im3D::Im3DContext *context = Im3D::GetCurrentContext();

        target->clearDepthBuffer();

        for (const std::pair<Im3DNativeMesh, std::vector<Matrix4>> &p : context->mNativeMeshes)
            target->renderInstancedMesh(RenderPassFlags_NoLighting, p.first, p.second);

        for (std::pair<const Im3DTextureId, Im3D::Im3DContext::RenderData> &p : context->mRenderData) {
            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                target->renderVertices(RenderPassFlags_NoLighting, i + 1, p.second.mVertices[i].data(), p.second.mVertices[i].size(), p.second.mIndices[i].data(), p.second.mIndices[i].size());
                target->renderVertices(RenderPassFlags_NoLighting, i + 1, p.second.mVertices2[i].data(), p.second.mVertices2[i].size(), p.second.mIndices2[i].data(), p.second.mIndices2[i].size(), p.first);
            }
        }
    }

}
}
