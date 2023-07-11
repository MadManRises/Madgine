#include "../clientlib.h"

#include "renderpass.h"

#include "renderdata.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Render::RenderPass)
METATABLE_END(Engine::Render::RenderPass)

namespace Engine {
namespace Render {

    void RenderPass::preRender(std::vector<Threading::TaskFuture<void>> &dependencies, RenderContext *context)
    {
        for (RenderData *dep : mDependencies)
            dependencies.push_back(dep->update(context));
    }

    const std::vector<RenderData *> &RenderPass::dependencies() const
    {
        return mDependencies;
    }

    void RenderPass::addDependency(RenderData *dep)
    {
        mDependencies.push_back(dep);
    }

    void RenderPass::removeDependency(RenderData *dep)
    {
        std::erase(mDependencies, dep);
    }

}
}