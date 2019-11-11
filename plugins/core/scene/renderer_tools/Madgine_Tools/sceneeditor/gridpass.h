#pragma once

#include "Madgine/render/renderpass.h"
/*#include "OpenGL/util/openglprogram.h"
#include "OpenGL/util/openglbuffer.h"
#include "OpenGL/util/openglvertexarray.h"*/
#include "meshloader.h"
#include "programloader.h"

namespace Engine {
namespace Tools {

    struct GridPass : Render::RenderPass {

        GridPass(Render::Camera *camera);

        virtual void render(Render::RenderTarget *target) override;

    private:
        Resources::MeshLoader::ResourceType *mMesh = nullptr;
        Render::ProgramLoader::ResourceType *mProgram = nullptr;

		Render::Camera *mCamera;
    };

}
}