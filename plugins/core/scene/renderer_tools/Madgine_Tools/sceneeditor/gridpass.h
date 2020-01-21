#pragma once

#include "Madgine/render/renderpass.h"
/*#include "OpenGL/util/openglprogram.h"
#include "OpenGL/util/openglbuffer.h"
#include "OpenGL/util/openglvertexarray.h"*/
#include "meshloader.h"
#include "programloader.h"

#include "Madgine/render/shadinglanguage/sl.h"

#define SL_SHADER grid
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Tools {

    struct GridPass : Render::RenderPass {

        GridPass(Render::Camera *camera, int priority);

        virtual void render(Render::RenderTarget *target) override;

		virtual int priority() const override;

    private:
        Render::MeshLoader::HandleType mMesh;
        Render::ProgramLoader::HandleType mProgram;

        GridPerFrame mParameters;

		Render::Camera *mCamera;

		int mPriority;
    };

}
}