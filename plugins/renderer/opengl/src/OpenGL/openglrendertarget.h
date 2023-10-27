#pragma once

#include "Madgine/render/rendertarget.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLRenderTarget : RenderTarget {

        OpenGLRenderTarget(OpenGLRenderContext *context, bool global, std::string name, bool flipFlop = false, RenderTarget *blitSource = nullptr);
        ~OpenGLRenderTarget();

        virtual void beginIteration(bool flipFlopping, size_t targetIndex, size_t targetCount, size_t targetSubresourceIndex) const override;
        virtual void endIteration() const override;

        virtual void clearDepthBuffer() override;

        virtual void setRenderSpace(const Rect2i &space) override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual Matrix4 getClipSpaceMatrix() const override;

        OpenGLRenderContext *context() const;
    };

}
}