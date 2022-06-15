#include "../clientlib.h"

#include "rendercontext.h"
#include "renderpass.h"
#include "rendertarget.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "gpumeshloader.h"

#include "meshdata.h"

METATABLE_BEGIN(Engine::Render::RenderTarget)
METATABLE_END(Engine::Render::RenderTarget)

namespace Engine {
namespace Render {

    RenderTarget::RenderTarget(RenderContext *context, bool global, std::string name, size_t iterations)
        : mContext(context)
        , mGlobal(global)
        , mName(std::move(name))
        , mIterations(iterations)
    {
        if (global)
            mContext->addRenderTarget(this);
    }

    RenderTarget::~RenderTarget()
    {
        for (RenderPass *pass : mRenderPasses) {
            pass->shutdown();
        }
        if (mGlobal)
            mContext->removeRenderTarget(this);
    }

    void RenderTarget::render()
    {
        if (mContext->frame() == mFrame)
            return;
        mFrame = mContext->frame();

        beginFrame();

        for (RenderPass *pass : mRenderPasses)
            pass->preRender();
        for (size_t iteration = 0; iteration < mIterations; ++iteration) {
            beginIteration(iteration);
            for (RenderPass *pass : mRenderPasses)
                pass->render(this, iteration);
            endIteration(iteration);
        }

        endFrame();
    }

    void RenderTarget::renderQuad(const PipelineInstance *pipeline)
    {
        static GPUMeshLoader::HandleType quad = GPUMeshLoader::loadManual("quad", {}, [](Render::GPUMeshLoader *loader, Render::GPUMeshData &data, Render::GPUMeshLoader::ResourceDataInfo &info) {
            std::vector<Compound<Render::VertexPos_3D>> vertices {
                { { -1, -1, 0 } },
                { { 1, -1, 0 } },
                { { -1, 1, 0 } },
                { { 1, 1, 0 } }
            };

            std::vector<unsigned short> indices {
                0, 1, 2, 1, 2, 3
            };

            return loader->generate(data, { 3, std::move(vertices), std::move(indices) });
        });

        if (quad.available())
            renderMesh(quad, pipeline);
    }

    void RenderTarget::addRenderPass(RenderPass *pass)
    {
        mRenderPasses.insert(
            std::upper_bound(mRenderPasses.begin(), mRenderPasses.end(), pass,
                [](RenderPass *first, RenderPass *second) { return first->priority() < second->priority(); }),
            pass);
        pass->setup(this);
    }

    void RenderTarget::removeRenderPass(RenderPass *pass)
    {
        pass->shutdown();
        std::erase(mRenderPasses, pass);
    }

    const std::vector<RenderPass *> &RenderTarget::renderPasses()
    {
        return mRenderPasses;
    }

    void RenderTarget::beginFrame()
    {
        if (!mName.empty())
            pushAnnotation(mName.c_str());
    }

    void RenderTarget::endFrame()
    {
        if (!mName.empty())
            popAnnotation();
    }

    void RenderTarget::beginIteration(size_t iteration) const
    {
    }

    void RenderTarget::endIteration(size_t iteration) const
    {
    }

    size_t RenderTarget::iterations() const
    {
        return mIterations;
    }

    RenderContext *RenderTarget::context() const
    {
        return mContext;
    }

    bool RenderTarget::resize(const Vector2i &size)
    {
        bool resized = resizeImpl(size);
        if (resized) {
            for (RenderPass *pass : mRenderPasses)
                pass->onTargetResize(size);
        }
        return resized;
    }

}
}