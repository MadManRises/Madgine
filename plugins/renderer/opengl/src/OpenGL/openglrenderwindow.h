#pragma once

#include "Modules/threading/threadlocal.h"
#include "openglrendertarget.h"
#include "util/openglssbobuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderWindow : OpenGLRenderTarget {
        OpenGLRenderWindow(OpenGLRenderContext *context, Window::OSWindow *w, size_t samples = 1, OpenGLRenderWindow *sharedContext = nullptr);
        ~OpenGLRenderWindow();

        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const override;
        virtual void endIteration() const override;

        virtual bool resizeImpl(const Vector2i &size) override;
        virtual Vector2i size() const override;

        ContextHandle mContext = 0;

    protected:
        SurfaceHandle getSurface() const;

    private:
        Window::OSWindow *mOsWindow;
        bool mReusedContext;

#if OPENGL_ES
#    if ANDROID || EMSCRIPTEN
        EGLSurface mSurface = nullptr;
#    endif

        static THREADLOCAL(OpenGLSSBOBufferStorage *) sCurrentSSBOBuffer;
        mutable std::optional<OpenGLSSBOBufferStorage> mSSBOBuffer;

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

REGISTER_TYPE(Engine::Render::OpenGLRenderWindow)