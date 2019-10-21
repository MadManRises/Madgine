#include "../../opengltoolslib.h"

#include "openglgridpass.h"

#include "OpenGL/openglshaderloader.h"

#include "Modules/math/matrix4.h"

#include "Madgine/scene/camera.h"
#include "Madgine/render/rendertarget.h"

namespace Engine {
namespace Tools {

    OpenGlGridPass::OpenGlGridPass()
    {
        std::shared_ptr<Render::OpenGLShader> vertexShader = Render::OpenGLShaderLoader::load("grid_VS");
        std::shared_ptr<Render::OpenGLShader> pixelShader = Render::OpenGLShaderLoader::load("grid_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get(), { "aPos" }))
            std::terminate();

        mVAO.bind();

        mVertexBuffer.bind(GL_ARRAY_BUFFER);
        mIndexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);

		mVAO.enableVertexAttribute(0, 4, GL_FLOAT, sizeof(Vector4), 0);
        
        Vector4 vertices[] = {
            { 0, 0, 0, 1 }, { 1, 0, 0, 0 }, { 0, 0, 1, 0 }, { -1, 0, 0, 0 }, { 0, 0, -1, 0 }/*,
            { 0, 0, 0, 1 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 0, 0, -1, 0 },
            { 0, 0, 0, 1 }, { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { -1, 0, 0, 0 }, { 0, -1, 0, 0 }*/
        };
        mVertexBuffer.setData(GL_ARRAY_BUFFER, sizeof(vertices), vertices);

        unsigned short indices[] = {
            0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1/*,
            5, 6, 7, 5, 7, 8, 5, 8, 9, 5, 9, 6,
            10, 11, 12, 10, 12, 13, 10, 13, 14, 10, 14, 11*/
        };
        mIndexBuffer.setData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices);
    }

    void OpenGlGridPass::render(Render::RenderTarget *target, Scene::Camera *camera)
    {
        const Vector2 &size = target->getSize();
        float aspectRatio = size.x / size.y;

        mProgram.setUniform("mvp", camera->getViewProjectionMatrix(aspectRatio));

        mVAO.bind();

        glDrawElements(
            GL_TRIANGLES, // mode
            12, // count
            GL_UNSIGNED_SHORT, // type
            (void *)0 // element array buffer offset
        );
        GL_CHECK();

    }
}
}