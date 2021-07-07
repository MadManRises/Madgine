#pragma once

namespace Engine {
namespace Render {

	struct RenderPass {
        virtual ~RenderPass() = default;

		virtual void setup(Render::RenderContext *context) {}
        virtual void shutdown() { }
        virtual void render(Render::RenderTarget *target) = 0;

		virtual int priority() const = 0;
	};

}
}