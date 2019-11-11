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

    GridPass::GridPass(Render::Camera *camera) : mCamera(camera)
    {

        mProgram = Render::ProgramLoader::getSingleton().getOrCreateManual("grid", [](Render::ProgramLoader::ResourceType *res) {
            return Render::ProgramLoader::getSingleton().create("grid");
        });


        mMesh = Resources::MeshLoader::getSingleton().getOrCreateManual("grid", [](Resources::MeshLoader::ResourceType *res) {
            Compound<Render::VertexPos_4D> vertices[] = {
                { { 0, 0, 0, 1 } },
                { { 1, 0, 0, 0 } },
                { { 0, 0, 1, 0 } },
                { { -1, 0, 0, 0 } },
                { { 0, 0, -1, 0 } }
            };

            unsigned short indices[] = {
                0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1
            };

            return Resources::MeshLoader::getSingleton().generate(3, vertices, 5, indices, 12);
        });
    }

    void GridPass::render(Render::RenderTarget *target)
    {
        Vector2i size = target->size();
        float aspectRatio = float(size.x) / size.y;

		std::shared_ptr<Render::Program> program = mProgram->loadData();
        Render::ProgramLoader::getSingleton().setUniform(*program, "mvp", mCamera->getViewProjectionMatrix(aspectRatio));
        Render::ProgramLoader::getSingleton().bind(*program);

        target->renderMesh(mMesh->loadData().get());
    }
}
}