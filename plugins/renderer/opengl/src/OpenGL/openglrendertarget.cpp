#include "opengllib.h"

#include "openglrendercontext.h"

#include "openglrendertarget.h"

#include "Meta/math/rect2i.h"

#include "Meta/math/matrix4.h"

namespace Engine {
namespace Render {

    OpenGLRenderTarget::OpenGLRenderTarget(OpenGLRenderContext *context, bool global, std::string name, size_t iterations, RenderTarget *blitSource)
        : RenderTarget(context, global, name, iterations, blitSource)
    {
    }

    OpenGLRenderTarget::~OpenGLRenderTarget()
    {
    }

    void OpenGLRenderTarget::beginIteration(size_t iteration) const
    {
        RenderTarget::beginIteration(iteration);

        Vector2i screenSize = size();

        if (screenSize.x <= 0)
            screenSize.x = 1;
        if (screenSize.y <= 0)
            screenSize.y = 1;

        glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
        GL_CHECK();

        if (!mBlitSource) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            GL_CHECK();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            GL_CHECK();
        }
    }

    void OpenGLRenderTarget::setRenderSpace(const Rect2i &space)
    {
        const Vector2i &screenSize = size();

        glViewport(space.mTopLeft.x, screenSize.y - (space.mTopLeft.y + space.mSize.y), space.mSize.x, space.mSize.y);
        GL_CHECK();
    }

    Matrix4 OpenGLRenderTarget::getClipSpaceMatrix() const
    {
        return Matrix4 { 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 2, -1,
            0, 0, 0, 1 };
    }

    void OpenGLRenderTarget::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }

    void OpenGLRenderTarget::pushAnnotation(const char *tag)
    {
#if OPENGL_ES
        //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, tag);
#else
        if (glPushDebugGroupKHR)
            glPushDebugGroupKHR(GL_DEBUG_SOURCE_APPLICATION, 0, -1, tag);
#endif
    }

    void OpenGLRenderTarget::popAnnotation()
    {
#if OPENGL_ES
        //glPopDebugGroup();
#else
        if (glPopDebugGroupKHR)
            glPopDebugGroupKHR();
#endif
    }

    OpenGLRenderContext *OpenGLRenderTarget::context() const
    {
        return static_cast<OpenGLRenderContext *>(RenderTarget::context());
    }

}
}
