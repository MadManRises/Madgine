#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "util/openglvertexarray.h"
#include "Madgine/render/vertexformat.h"

namespace Engine {
namespace Render {

    ContextHandle createContext(Window::OSWindow *window, size_t samples, ContextHandle reusedContext, bool setup = true);
    void destroyContext(Window::OSWindow *window, ContextHandle context, bool reusedContext = false);
    void makeCurrent(Window::OSWindow *window, ContextHandle context);
    void swapBuffers(Window::OSWindow *window, ContextHandle context);

    struct MADGINE_OPENGL_EXPORT OpenGLRenderContext : public RenderContextComponent<OpenGLRenderContext> {
        OpenGLRenderContext(Threading::TaskQueue *queue);
        ~OpenGLRenderContext();
        OpenGLRenderContext(const OpenGLRenderContext &) = delete;

        OpenGLRenderContext& operator=(const OpenGLRenderContext &) = delete;

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual void unloadAllResources() override;

        static OpenGLRenderContext &getSingleton();

        virtual bool supportsMultisampling() const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        void bindFormat(VertexFormat format, OpenGLBuffer *instanceBuffer, size_t instanceDataSize);
        void unbindFormat();

    private:
        std::map<VertexFormat, OpenGLVertexArray> mVAOs;
    };

}
}

REGISTER_TYPE(Engine::Render::OpenGLRenderContext)