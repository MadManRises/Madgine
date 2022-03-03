#include "../scenerenderertoolslib.h"

#include "gridpass.h"

//#include "OpenGL/openglshaderloader.h"

#include "meshloader.h"

#include "Meta/math/matrix4.h"

#include "Madgine/render/camera.h"
#include "Madgine/render/rendertarget.h"
#include "render/vertex.h"

#include "gpumeshloader.h"

#include "programloader.h"

namespace Engine {
namespace Tools {

    GridPass::GridPass(Render::Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {

        mProgram.create("grid", { 0, sizeof(GridPerFrame) });

        mMesh = Render::GPUMeshLoader::loadManual("grid", {}, [](Render::GPUMeshLoader *loader, Render::GPUMeshData &data, Render::GPUMeshLoader::ResourceDataInfo &info) {
            std::vector<Compound<Render::VertexPos_4D>> vertices {
                { { 0, 0, 0, 1 } },
                { { 1, 0, 0, 0 } },
                { { 0, 0, 1, 0 } },
                { { -1, 0, 0, 0 } },
                { { 0, 0, -1, 0 } }
            };

            std::vector<unsigned short> indices {
                0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1
            };

            return loader->generate(data, { 3, std::move(vertices), std::move(indices) });
        });
    }

    void GridPass::render(Render::RenderTarget *target, size_t iteration)
    {
        /* if (!mProgram.available())
            return;*/

        target->pushAnnotation("Grid");

        Vector2i size = target->size();
        float aspectRatio = float(size.x) / size.y;
        {
            auto parameters = mProgram.mapParameters(1).cast<GridPerFrame>();

            parameters->vp = mCamera->getViewProjectionMatrix(aspectRatio);
        }

        target->renderMesh(mMesh, mProgram);

        target->popAnnotation();
    }

    int GridPass::priority() const
    {
        return mPriority;
    }
}
}