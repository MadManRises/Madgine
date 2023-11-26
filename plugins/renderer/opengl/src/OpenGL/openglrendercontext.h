#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/render/vertexformat.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "util/openglvertexarray.h"
#include "util/openglheapallocator.h"

#include "Generic/allocator/bump.h"
#include "Generic/allocator/bucket.h"
#include "Generic/allocator/fixed.h"
#include "Generic/allocator/heap.h"

namespace Engine {
namespace Render {

    ContextHandle createContext(SurfaceHandle surface, size_t samples, ContextHandle reusedContext, bool setup = true);
    void destroyContext(SurfaceHandle surface, ContextHandle context, bool reusedContext = false);
    void makeCurrent(SurfaceHandle surface, ContextHandle context);
    void swapBuffers(SurfaceHandle surface, ContextHandle context);

    struct MADGINE_OPENGL_EXPORT OpenGLRenderContext : public RenderContextComponent<OpenGLRenderContext> {
        OpenGLRenderContext(Threading::TaskQueue *queue);
        ~OpenGLRenderContext();
        OpenGLRenderContext(const OpenGLRenderContext &) = delete;

        OpenGLRenderContext &operator=(const OpenGLRenderContext &) = delete;

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual Threading::Task<void> unloadAllResources() override;

        static OpenGLRenderContext &getSingleton();

        virtual bool supportsMultisampling() const override;

        virtual UniqueResourceBlock createResourceBlock(std::vector<const Texture *> textures) override;

        void bindFormat(VertexFormat format, OpenGLBuffer *instanceBuffer, size_t instanceDataSize);
        void unbindFormat();

        OpenGLHeapAllocator mBufferMemoryHeap;
        LogBucketAllocator<HeapAllocator<OpenGLHeapAllocator &>, 64, 4096, 4> mBufferAllocator;

        OpenGLMappedHeapAllocator mTempMemoryHeap;
        BumpAllocator<FixedAllocator<OpenGLMappedHeapAllocator &>> mTempAllocator;

    private:
        std::map<VertexFormat, OpenGLVertexArray> mVAOs;
    };

}
}

REGISTER_TYPE(Engine::Render::OpenGLRenderContext)