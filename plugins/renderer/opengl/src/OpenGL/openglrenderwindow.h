#pragma once

#include "Madgine/render/renderwindow.h"

#include "openglshaderprogram.h"

#include "util/opengltexture.h"
#include "util/openglvertexarray.h"
#include "util/openglbuffer.h"

#include "Modules/math/atlas2.h"

#include "imageloaderlib.h"
#include "imageloader.h"

namespace Engine {
namespace Render {

    class MADGINE_OPENGL_EXPORT OpenGLRenderWindow : public RenderWindow {
    public:
        OpenGLRenderWindow(Window::Window *w, ContextHandle context, GUI::TopLevelWindow *mTopLevelWindow = nullptr);
        ~OpenGLRenderWindow();

        virtual void render() override;
        virtual void makeCurrent() override;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(Scene::Camera *camera, const Vector2 &size) override;

        const ContextHandle mContext;
    private:

		void expandUIAtlas();

        OpenGLShaderProgram mProgram;

        OpenGLTexture mDefaultTexture;
        OpenGLTexture mUIAtlasTexture;
        Atlas2 mUIAtlas;
        int mUIAtlasSize = 0;
        std::map<Resources::ImageLoader::ResourceType *, Atlas2::Entry> mUIAtlasEntries;

		OpenGLVertexArray mVAO;
        OpenGLBuffer mVBO;

		GUI::TopLevelWindow *mTopLevelWindow = nullptr;
    };

}
}