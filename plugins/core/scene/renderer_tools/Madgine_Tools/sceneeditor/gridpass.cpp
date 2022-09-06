#include "../scenerenderertoolslib.h"

#include "gridpass.h"

//#include "OpenGL/openglshaderloader.h"

#include "Madgine/meshloader/meshloader.h"

#include "Meta/math/matrix4.h"

#include "Madgine/render/camera.h"
#include "Madgine/render/rendertarget.h"
#include "Madgine/render/vertex.h"

#include "Madgine/meshloader/gpumeshloader.h"

namespace Engine {
namespace Tools {

    GridPass::GridPass(Render::Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {

        mMesh.load("Plane");

        mPipeline.createStatic({ .vs = "grid", .ps = "grid", .format = type_holder<Compound<Render::VertexPos_4D>>, .bufferSizes = { 0, sizeof(GridPerFrame) } });

    }

    void GridPass::render(Render::RenderTarget *target, size_t iteration)
    {
        if (!mPipeline.available())
            return;
        if (!mMesh.available())
            return;

        target->pushAnnotation("Grid");

        Vector2i size = target->size();
        float aspectRatio = float(size.x) / size.y;
        {
            auto parameters = mPipeline->mapParameters<GridPerFrame>(1);

            parameters->vp = mCamera->getViewProjectionMatrix(aspectRatio);
        }

        mPipeline->renderMesh(mMesh);

        target->popAnnotation();
    }

    int GridPass::priority() const
    {
        return mPriority;
    }
}
}