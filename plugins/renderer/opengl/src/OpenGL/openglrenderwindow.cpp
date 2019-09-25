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

#include "imagedata.h"

namespace Engine {
namespace Render {

    OpenGLRenderWindow::OpenGLRenderWindow(Window::Window *w, ContextHandle context, GUI::TopLevelWindow *topLevelWindow)
        : RenderWindow(w)
        , mContext(context)
        , mUIAtlas({ 512, 512 })
        , mTopLevelWindow(topLevelWindow)
    {
        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("ui_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("ui_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get()))
            throw 0;

        mProgram.setUniform("texture", 0);

        mVAO.bind();

        mVBO.bind(GL_ARRAY_BUFFER);

        mVAO.enableVertexAttribute(0, &GUI::Vertex::mPos);
        mVAO.enableVertexAttribute(1, &GUI::Vertex::mColor);
        mVAO.enableVertexAttribute(2, &GUI::Vertex::mUV);

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

            std::map<uint32_t, std::vector<GUI::Vertex>> vertices;

            std::queue<std::pair<GUI::WidgetBase *, int>> q;
            for (GUI::WidgetBase *w : mTopLevelWindow->widgets()) {
                if (w->mVisible) {
                    q.push(std::make_pair(w, 0));
                }
            }
            while (!q.empty()) {

                GUI::WidgetBase *w = q.front().first;
                int depth = q.front().second;
                q.pop();

                for (GUI::WidgetBase *c : w->children()) {
                    if (c->mVisible)
                        q.push(std::make_pair(c, depth + 1));
                }

                std::pair<std::vector<GUI::Vertex>, uint32_t> localVertices = w->vertices(screenSize);

                Resources::ImageLoader::ResourceType *resource = w->resource();
                if (resource) {
                    auto it = mUIAtlasEntries.find(resource);
                    if (it == mUIAtlasEntries.end()) {
                        std::shared_ptr<Resources::ImageData> data = resource->loadData();
                        it = mUIAtlasEntries.try_emplace(resource, mUIAtlas.insert({ data->mWidth, data->mHeight }, [this]() { expandUIAtlas(); })).first;
                        mUIAtlasTexture.setSubData({ it->second.mArea.mTopLeft.x, it->second.mArea.mTopLeft.y }, it->second.mArea.mSize, data->mBuffer, GL_UNSIGNED_BYTE);
                    }

                    std::transform(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]), [&](const GUI::Vertex &v) {
                        return GUI::Vertex {
                            v.mPos,
                            v.mColor,
                            { (it->second.mArea.mSize.x / (512.f * mUIAtlasSize)) * v.mUV.x + it->second.mArea.mTopLeft.x / (512.f * mUIAtlasSize),
                                (it->second.mArea.mSize.y / (512.f * mUIAtlasSize)) * v.mUV.y + it->second.mArea.mTopLeft.y / (512.f * mUIAtlasSize) }
                        };
                    });

                } else {
                    std::move(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]));
                }
            }

            mVAO.bind();

            for (const std::pair<const uint32_t, std::vector<GUI::Vertex>> &p : vertices) {
                if (!p.second.empty()) {

                    glBindTexture(GL_TEXTURE_2D, p.first != std::numeric_limits<uint32_t>::max() ? p.first : mUIAtlasTexture.handle());

                    mVBO.setData(GL_ARRAY_BUFFER, sizeof(p.second[0]) * p.second.size(), p.second.data());

                    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(p.second.size())); // Starting from vertex 0; 3 vertices total -> 1 triangle
                    GL_CHECK();
                }
            }
        }
    }

    void OpenGLRenderWindow::makeCurrent()
    {
        Engine::Render::makeCurrent(window(), mContext);
    }

    std::unique_ptr<RenderTarget> OpenGLRenderWindow::createRenderTarget(Scene::Camera *camera, const Vector2 &size)
    {
        return std::make_unique<OpenGLRenderTexture>(this, camera, size);
    }

    void OpenGLRenderWindow::expandUIAtlas()
    {
        if (mUIAtlasSize == 0) {
            mUIAtlasSize = 4;
            mUIAtlasTexture.setData({ mUIAtlasSize * 512, mUIAtlasSize * 512 }, nullptr, GL_UNSIGNED_BYTE);
            for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * y });
                }
			}
        } else {
            /*for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * (y + mUIAtlasSize) });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * y });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * (y + mUIAtlasSize) });
                }
            }
            mUIAtlasSize *= 2;
            mUIAtlasTexture.resize({ 512 * mUIAtlasSize, 512 * mUIAtlasSize });*/
            throw "TODO";
        }
    }

}
}