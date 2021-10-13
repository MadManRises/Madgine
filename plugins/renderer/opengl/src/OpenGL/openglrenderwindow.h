#pragma once

#include "Modules/threading/threadlocal.h"
#include "openglrendertarget.h"
#include "util/openglssbobuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderWindow : OpenGLRenderTarget {
        OpenGLRenderWindow(OpenGLRenderContext *context, Window::OSWindow *w, size_t samples = 1, OpenGLRenderWindow *sharedContext = nullptr);
        ~OpenGLRenderWindow();

        virtual void beginIteration(size_t iteration) override;
        virtual void endIteration(size_t iteration) override;

        virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;
        virtual size_t textureCount() const override;
        virtual TextureDescriptor depthTexture() const override;

        virtual bool resizeImpl(const Vector2i &size) override;
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