#pragma once

#include "Madgine/render/rendertarget.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTarget : RenderTarget {

        OpenGLRenderTarget(OpenGLRenderContext *context, bool global, std::string name, size_t iterations = 1);
        ~OpenGLRenderTarget();

        virtual void beginIteration(size_t iteration) const override;

        virtual void clearDepthBuffer() override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;
    };

}
}