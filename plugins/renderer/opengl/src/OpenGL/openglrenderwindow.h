#pragma once

#include "Madgine/render/renderwindowcollector.h"

#include "openglshaderprogram.h"

#include "util/opengltexture.h"
#include "util/openglvertexarray.h"
#include "util/openglbuffer.h"

#include "Modules/math/atlas2.h"

#include "imageloaderlib.h"
#include "imageloader.h"

namespace Engine {
namespace Render {

    class MADGINE_OPENGL_EXPORT OpenGLRenderWindow : public RenderWindowComponent<OpenGLRenderWindow> {
    public:
        OpenGLRenderWindow(Window::Window *w, GUI::TopLevelWindow *topLevel, RenderWindow *reusedResources = nullptr);
        ~OpenGLRenderWindow();

        virtual void render() override;
        virtual void makeCurrent() override;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(Scene::Camera *camera, const Vector2 &size) override;

        ContextHandle mContext = 0;
    private:

		void expandUIAtlas();

        OpenGLShaderProgram mProgram;

        OpenGLTexture mDefaultTexture = dont_create;
        OpenGLTexture mUIAtlasTexture = dont_create;
        Atlas2 mUIAtlas;
        int mUIAtlasSize = 0;
        std::map<Resources::ImageLoader::ResourceType *, Atlas2::Entry> mUIAtlasEntries;

		OpenGLVertexArray mVAO = dont_create;
        OpenGLBuffer mVBO = dont_create;

		GUI::TopLevelWindow *mTopLevelWindow = nullptr;
        bool mReusedContext;
    };

}
}