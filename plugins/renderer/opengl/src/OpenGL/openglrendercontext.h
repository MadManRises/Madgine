#pragma once

#include "Madgine/render/rendercontextcollector.h"

#include "util/openglprogram.h"
#include "util/opengltexture.h"
#include "util/openglvertexarray.h"
#include "util/openglbuffer.h"

#include "Modules/math/atlas2.h"

#include "imageloaderlib.h"
#include "imageloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderContext : public RenderContextComponent<OpenGLRenderContext> {
        OpenGLRenderContext();
        ~OpenGLRenderContext();

		virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::Window *w) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }) override;

    };

}
}