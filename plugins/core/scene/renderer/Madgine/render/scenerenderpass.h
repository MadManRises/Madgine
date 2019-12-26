#pragma once 

#include "Madgine/render/renderpass.h"

#include "programloader.h"

#include "Madgine/render/camera.h"

namespace Engine {
namespace Render {

	struct MADGINE_SCENE_RENDERER_EXPORT SceneRenderPass : RenderPass {
        SceneRenderPass(Camera *camera, int priority);
		
		virtual void render(Render::RenderTarget *target) override;

		virtual int priority() const override;

	private:
		ProgramLoader::HandleType mProgram;

		Camera *mCamera;

		int mPriority;
    };

}
}