#include "../scenerenderertoolslib.h"

#include "gridpass.h"

//#include "OpenGL/openglshaderloader.h"

#include "Modules/math/matrix4.h"

#include "Madgine/render/camera.h"
#include "Madgine/render/rendertarget.h"
#include "Modules/render/vertex.h"

#include "meshloader.h"

#include "program.h"
#include "programloader.h"

namespace Engine {
namespace Tools {

    GridPass::GridPass(Render::Camera *camera, int priority)
        : mCamera(camera)
        , mPriority(priority)
    {

        mProgram.create("grid");

        mMesh = Render::MeshLoader::loadManual("grid", {}, [](Render::MeshLoader *loader, Render::MeshData &data, Render::MeshLoader::ResourceType * res) {
            std::vector<Compound<Render::VertexPos_4D>> vertices{
                { { 0, 0, 0, 1 } },
                { { 1, 0, 0, 0 } },
                { { 0, 0, 1, 0 } },
                { { -1, 0, 0, 0 } },
                { { 0, 0, -1, 0 } }
            };

            std::vector<unsigned short> indices{
                0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1
            };

            return loader->generate(data, 3, std::move(vertices), std::move(indices));
        });
    }

    void GridPass::render(Render::RenderTarget *target)
    {
        Vector2i size = target->size();
        float aspectRatio = float(size.x) / size.y;

        mProgram.setUniform("mvp", mCamera->getViewProjectionMatrix(aspectRatio));
        mProgram.bind();

        target->renderMesh(mMesh);
    }

    int GridPass::priority() const
    {
        return mPriority;
    }
}
}