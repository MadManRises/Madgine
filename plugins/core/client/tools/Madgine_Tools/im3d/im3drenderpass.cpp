#include "../clienttoolslib.h"

#include "im3drenderpass.h"

#include "Madgine/render/rendertarget.h"

#include "im3d/im3d.h"
#include "im3d/im3d_internal.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Madgine/render/texturedescriptor.h"

#include "Madgine/meshloader/meshdata.h"

#define SL_SHADER shaders/im3d
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Render {

    Im3DRenderPass::Im3DRenderPass(Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {
        mPipeline.create({ .vs = "im3d", .ps = "im3d", .bufferSizes = { sizeof(Im3DPerApplication), 0, sizeof(Im3DPerObject) } });

        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            mMeshes[i][0].create({ i + 1, std::vector<Im3D::Vertex> {} });
            mMeshes[i][1].create({ i + 1, std::vector<Im3D::Vertex2> {} });
        }
    }

    void Im3DRenderPass::render(RenderTarget *target, size_t iteration)
    {
        if (!mPipeline.available())
            return;

        target->pushAnnotation("Im3D");

        Im3D::Im3DContext *context = Im3D::GetCurrentContext();

        target->clearDepthBuffer();

        Vector2i size = target->size();

        float aspectRatio = float(size.x) / size.y;

        {
            auto perApplication = mPipeline->mapParameters<Im3DPerApplication>(0);

            perApplication->p = target->getClipSpaceMatrix() * mCamera->getProjectionMatrix(aspectRatio);
        }

        /*for (const std::pair<Im3DNativeMesh, std::vector<Matrix4>> &p : context->mNativeMeshes)
            target->renderInstancedMesh(RenderPassFlags_NoLighting, p.first, p.second);*/

        for (std::pair<const Im3DTextureId, Im3D::Im3DContext::RenderData> &p : context->mRenderData) {

            mPipeline->bindTextures({ { p.first, Render::TextureType_2D } });

            {
                auto perObject = mPipeline->mapParameters<Im3DPerObject>(2);

                perObject->hasDistanceField = false;

                perObject->mv = mCamera->getViewMatrix();

                perObject->hasTexture = p.first != 0;
                perObject->hasDistanceField = bool(p.second.mFlags & RenderPassFlags_DistanceField);
            }

            mPipeline->bindTextures({ { p.first, TextureType_2D } });

            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                if (!p.second.mVertices[i].empty()) {
                    mMeshes[i][0].update({ i + 1, p.second.mVertices[i], p.second.mIndices[i] });
                    mPipeline->renderMesh(mMeshes[i][0]);
                }
                
                if (!p.second.mVertices2[i].empty()) {
                    mMeshes[i][1].update({ i + 1, p.second.mVertices2[i], p.second.mIndices2[i] });
                    mPipeline->renderMesh(mMeshes[i][1]);
                }
            }
        }

        target->popAnnotation();
    }

    int Im3DRenderPass::priority() const
    {
        return mPriority;
    }
}
}
