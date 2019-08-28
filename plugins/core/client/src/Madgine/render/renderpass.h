#pragma once

namespace Engine {
namespace Render {

	struct RenderPass {
        virtual ~RenderPass() = default;

        virtual void render(Scene::Camera *camera, const Vector2 &size) = 0;
	};

}
}