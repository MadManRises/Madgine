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

        GridPass(Render::Camera *camera, int priority);

        virtual void render(Render::RenderTarget *target) override;

		virtual int priority() const override;

    private:
        Resources::MeshLoader::HandleType mMesh;
        Render::ProgramLoader::HandleType mProgram;

		Render::Camera *mCamera;

		int mPriority;
    };

}
}