#include "../scenerenderertoolslib.h"

#include "gridpass.h"

//#include "OpenGL/openglshaderloader.h"

#include "Madgine/meshloader/meshloader.h"

#include "Meta/math/matrix4.h"

#include "Madgine/render/camera.h"
#include "Madgine/render/rendertarget.h"
#include "Madgine/render/vertex.h"

#include "Madgine/meshloader/gpumeshloader.h"


#include "Madgine/render/shadinglanguage/sl.h"

#define SL_SHADER shaders/grid
#include INCLUDE_SL_SHADER


namespace Engine {
namespace Tools {

    GridPass::GridPass(Render::Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {

        mMesh.load("Plane");

        mPipeline.create({ .vs = "grid", .ps = "grid", .bufferSizes = { 0, sizeof(GridPerFrame) } });

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

            parameters->vp = target->getClipSpaceMatrix() * mCamera->getViewProjectionMatrix(aspectRatio);
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