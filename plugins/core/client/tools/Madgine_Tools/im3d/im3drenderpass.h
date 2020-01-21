#pragma once

#include "Madgine/render/renderpass.h"

#include "programloader.h"

#include "Madgine/render/shadinglanguage/sl.h"

#define SL_SHADER im3d
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_TOOLS_EXPORT Im3DRenderPass : RenderPass {
        Im3DRenderPass(Camera *camera, int priority);

        virtual void render(RenderTarget *target) override;

        virtual int priority() const override;

    private:
        ProgramLoader::HandleType mProgram;

        Camera *mCamera;

        int mPriority;

        Im3DPerApplication mPerApplication;
        Im3DPerFrame mPerFrame;
        Im3DPerObject mPerObject;
    };

}
}