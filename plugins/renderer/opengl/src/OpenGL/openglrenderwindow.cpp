#include "opengllib.h"
#include "openglrenderwindow.h"
#include "Madgine/gui/widgets/toplevelwindow.h"
#include "openglrenderer.h"

#include <iostream>

#undef min
#undef max

#include "Modules/debug/profiler/profiler.h"

#include "Modules/math/vector3.h"
#include "Modules/math/vector4.h"

#include "Madgine/gui/widgets/widget.h"

#include "Madgine/gui/vertex.h"

#include "openglrendertexture.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Interfaces/window/windowapi.h"

namespace Engine {
namespace Render {

    static GLuint vertexbuffer = 0;

    static GLuint vao;

    OpenGLRenderWindow::OpenGLRenderWindow(Window::Window *w, ContextHandle context, GUI::TopLevelWindow *topLevelWindow)
        : RenderWindow(w)
        , mContext(context)
        , mTopLevelWindow(topLevelWindow)
    {
        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("ui_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("ui_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get()))
            throw 0;

        mProgram.setUniform("textures[0]", 0);
        mProgram.setUniform("textures[1]", 1);

        glGenVertexArrays(1, &vao);
        GL_CHECK();
        glBindVertexArray(vao);
        GL_CHECK();

        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);
        GL_CHECK();

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        GL_CHECK();
        glVertexAttribPointer(
            0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(GUI::Vertex), // stride
            (void *)offsetof(GUI::Vertex, mPos) // array buffer offset
        );
        glEnableVertexAttribArray(0);
        GL_CHECK();
        glVertexAttribPointer(
            1, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            4, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(GUI::Vertex), // stride
            (void *)offsetof(GUI::Vertex, mColor) // array buffer offset
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            2, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            sizeof(GUI::Vertex), // stride
            (void *)offsetof(GUI::Vertex, mUV) // array buffer offset
        );
        glEnableVertexAttribArray(2);
        /*glVertexAttribPointer(
            3, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            1, // size
            GL_UNSIGNED_INT, // type
            GL_FALSE,
            sizeof(GUI::Vertex), // stride
            (void *)offsetof(GUI::Vertex, mTextureIndex) // array buffer offset
        );
        glEnableVertexAttribArray(3);*/

        mDefaultTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        Vector4 borderColor = { 1, 1, 1, 1 };
        mDefaultTexture.setData({ 1, 1 }, &borderColor);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        //glDepthRange(0.0, 1.0);
    }

    OpenGLRenderWindow::~OpenGLRenderWindow()
    {
        shutdownWindow(window(), mContext);
    }

    void OpenGLRenderWindow::render()
    {
        PROFILE();

        updateRenderTargets();
        glActiveTexture(GL_TEXTURE0);
        glCheck();
        mDefaultTexture.bind();

        Vector2 actualScreenSize { static_cast<float>(window()->renderWidth()), static_cast<float>(window()->renderHeight()) };
        Vector3 screenPos, screenSize;
        if (mTopLevelWindow) {
            std::tie(screenPos, screenSize) = mTopLevelWindow->getAvailableScreenSpace();
        } else {
            screenPos = Vector3::ZERO;
            screenSize = { actualScreenSize.x,
                actualScreenSize.y,
                1.0f };
        }

        glViewport(static_cast<GLsizei>(screenPos.x), static_cast<GLsizei>(actualScreenSize.y - screenPos.y - screenSize.y), static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (mTopLevelWindow) {

            mProgram.bind();

            std::vector<GUI::Vertex> vertices;

            std::queue<std::pair<GUI::Widget *, int>> q;
            for (GUI::Widget *w : mTopLevelWindow->widgets()) {
                if (w->isVisible()) {
                    q.push(std::make_pair(w, 0));
                }
            }
            while (!q.empty()) {

                GUI::Widget *w = q.front().first;
                int depth = q.front().second;
                q.pop();

                for (GUI::Widget *c : w->children()) {
                    if (c->isVisible())
                        q.push(std::make_pair(c, depth + 1));
                }

                std::vector<GUI::Vertex> localVertices = w->vertices(screenSize);

                std::move(localVertices.begin(), localVertices.end(), std::back_inserter(vertices));
            }

            if (!vertices.empty()) {

                glBindVertexArray(vao);
                GL_CHECK();

                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                GL_CHECK();

                glVertexAttribPointer(
                    0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    3, // size
                    GL_FLOAT, // type
                    GL_FALSE, // normalized?
                    sizeof(GUI::Vertex), // stride
                    (void *)offsetof(GUI::Vertex, mPos) // array buffer offset
                );
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(
                    1, // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    4, // size
                    GL_FLOAT, // type
                    GL_FALSE, // normalized?
                    sizeof(GUI::Vertex), // stride
                    (void *)offsetof(GUI::Vertex, mColor) // array buffer offset
                );
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(
                    2, // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    2, // size
                    GL_FLOAT, // type
                    GL_FALSE, // normalized?
                    sizeof(GUI::Vertex), // stride
                    (void *)offsetof(GUI::Vertex, mUV) // array buffer offset
                );
                glEnableVertexAttribArray(2);

                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STREAM_DRAW);
                GL_CHECK();

                // Draw the triangle !
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size())); // Starting from vertex 0; 3 vertices total -> 1 triangle
                GL_CHECK();
            }
        }
    }

    void OpenGLRenderWindow::makeCurrent()
    {
        Engine::Render::makeCurrent(window(), mContext);
    }

    std::unique_ptr<RenderTarget> OpenGLRenderWindow::createRenderTarget(Scene::Camera *camera, const Vector2 &size)
    {
        return std::make_unique<OpenGLRenderTexture>(this, ++mTextureCount, camera, size);
    }

}
}