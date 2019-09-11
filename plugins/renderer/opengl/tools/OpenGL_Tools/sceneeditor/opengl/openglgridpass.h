#pragma once

#include "Madgine/render/renderpass.h"
#include "OpenGL/openglshaderprogram.h"
#include "OpenGL/util/openglbuffer.h"
#include "OpenGL/util/openglvertexarray.h"

namespace Engine {
namespace Tools {

	struct OpenGlGridPass : Render::RenderPass {

		OpenGlGridPass();

        virtual void render(Render::RenderTarget *target, Scene::Camera *camera) override;

	private:
        Render::OpenGLShaderProgram mProgram;        
        Render::OpenGLBuffer mVertexBuffer;
        Render::OpenGLBuffer mIndexBuffer;
        Render::OpenGLVertexArray mVAO;
    };

}
}