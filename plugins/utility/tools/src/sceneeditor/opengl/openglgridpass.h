#pragma once

#include "Madgine/render/renderpass.h"
#include "OpenGL/openglshaderprogram.h"
#include "OpenGL/util/openglbuffer.h"

namespace Engine {
namespace Tools {

	struct OpenGlGridPass : Render::RenderPass {

		OpenGlGridPass();

        virtual void render(Scene::Camera *camera, const Vector2 &size) override;

	private:
        Render::OpenGLShaderProgram mProgram;
        unsigned int mVAO;
        Render::OpenGLBuffer mVertexBuffer;
        Render::OpenGLBuffer mIndexBuffer;
    };

}
}