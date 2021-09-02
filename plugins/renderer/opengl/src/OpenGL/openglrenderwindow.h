#pragma once

#include "Modules/threading/threadlocal.h"
#include "openglrendertarget.h"
#include "util/openglssbobuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderWindow : OpenGLRenderTarget {
        OpenGLRenderWindow(OpenGLRenderContext *context, Window::OSWindow *w, OpenGLRenderWindow *sharedContext = nullptr);
        ~OpenGLRenderWindow();

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual TextureHandle texture() const override;
        virtual TextureHandle depthTexture() const override;

        virtual bool resize(const Vector2i &size) override;
        virtual Vector2i size() const override;

        ContextHandle mContext = 0;

    private:
        Window::OSWindow *mOsWindow;
        bool mReusedContext;

#if OPENGL_ES
        static THREADLOCAL(OpenGLSSBOBufferStorage *) sCurrentSSBOBuffer;
        OpenGLSSBOBufferStorage mSSBOBuffer;

    public:
        static OpenGLSSBOBufferStorage &getSSBOStorage()
        {
            assert(sCurrentSSBOBuffer);
            return *sCurrentSSBOBuffer;
        }
#endif
    };

}
}

RegisterType(Engine::Render::OpenGLRenderWindow);