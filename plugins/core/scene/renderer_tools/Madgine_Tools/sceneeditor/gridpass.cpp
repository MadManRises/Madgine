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

        mMesh.load("Plane");
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