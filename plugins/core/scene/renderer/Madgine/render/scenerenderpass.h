#pragma once 

#include "Madgine/render/renderpass.h"

#include "programloader.h"

#include "Madgine/render/camera.h"

namespace Engine {
namespace Render {

	struct MADGINE_SCENE_RENDERER_EXPORT SceneRenderPass : RenderPass {
        SceneRenderPass(Camera *camera);
		
		virtual void render(Render::RenderTarget *target) override;

	private:
		ProgramLoader::ResourceType *mProgram;

		Camera *mCamera;
    };

}
}